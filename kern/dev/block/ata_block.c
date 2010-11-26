#include <stdint.h>
#include <dev/pci/pci_access.h>
#include <mm/malloc.h>
#include "ata.h"

static void ata_dump_identity(struct ata_channel *ata);
static uint8_t ata_read_status(struct ata_channel *ata);
static uint8_t ata_wait_for(struct ata_channel *ata, uint8_t mask);
static uint8_t ata_wait_clear(struct ata_channel *ata, uint8_t mask);
static void ata_write_dev(struct ata_channel *ata, uint8_t devdata);
static void ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd);
static void possibly_update_dev(struct ata_channel *ata);

extern uint8_t ata_read8_io(struct ata_channel *ata, uint8_t which);
extern uint16_t ata_read16_io(struct ata_channel *ata, uint8_t which);
extern void ata_write8_io(struct ata_channel *ata, uint8_t which, uint8_t data);
extern void ata_write16_io(struct ata_channel *ata, uint8_t which, uint16_t data); 

#define MAX_CHANNEL 5
static struct ata_channel ata_channels[MAX_CHANNEL + 1];
static struct ide_bus ide_busses[(MAX_CHANNEL + 1) / 2];
static int total_channel = 0;

alloc_ata_dev() {
	KASSERT(total_channel < MAX_CHANNEL, ("not enough channel structures\n"));
	ata_channels[total_channel].id = total_channel;
	ata_channels[total_channel].idebus = &ide_busses[total_channel / 2];
	total_channel++;
	return &ata_channels[total_channel-1];
}

static int
ata_pio_read(struct ata_channel *ata, uint16_t *buf, uint64_t words) {
	volatile uint8_t status;
	while(words) {
		do {
			status = ata_read_status(ata);
		} while(!(status & ATA_STS_DRQ));

		if (!(status & ATA_STS_DRDY)) {
			printf("Fatal ATA error\n");
			return -1;
		}

		*buf = ata->read_port16(ata, ATA_DATA_OFF);
		buf++;
		words--;
	}
	return 0;
}

int
ata_read_sectors(struct ata_channel *ata, void *buf, uint64_t lba, uint64_t count) {
	if (count > 256) {
		printf("Count > 256 sectors is not yet supported\n");
	}
}

static void
ata_do_identify(struct ata_channel *ata) {
	if (ata->identify_data != NULL) {
		printf("Warning, identify was already executed on ATA channel %d\n", ata->id);
	} else {
		ata->identify_data = malloc(sizeof(struct ata_identify_data));
	}

	// OSDev says use a0 and b0, but bochs doesn't like it
	//ata_write_dev(ata, 0xa0 | ata->devid);
	possibly_update_dev(ata);

	ata->write_port8(ata, ATA_SECTCOUNT_OFF, 0);
	ata->write_port8(ata, ATA_LBAL_OFF, 0);
	ata->write_port8(ata, ATA_LBAM_OFF, 0);
	ata->write_port8(ata, ATA_LBAH_OFF, 0);

	// Wait for DRDY to issue the identify command
	ata_wait_for(ata, ATA_STS_DRDY);

	ata_write_cmd(ata, ATA_CMD_IDENTIFY);

	uint8_t status = 0;
	status = ata_read_status(ata);
	if (status == 0)
		printf("HELP, device doesn't exist?");

	// Wait until no longer busy
	ata_wait_clear(ata, ATA_STS_BSY);

	// According to OSDev, we need to check LBAM and LBAH for some older packet devices
	if (ata->read_port8(ata, ATA_LBAM_OFF) || ata->read_port8(ata, ATA_LBAH_OFF))
		printf("HELP, device isn't ATA?");

	uint8_t error;
	while(1) {
		status = ata_read_status(ata);
		if (status & ATA_STS_DRQ)
			break;
		error = ata->read_port8(ata, ATA_ERROR_OFF);
		// TODO: why do this?
		if (error & 1) {
			printf("Bad again\n");
			break;
		}
	}

	// size is in words, so divide by 2
	ata_pio_read(ata, (uint8_t *)ata->identify_data, sizeof(struct ata_identify_data) / 2);
}

/* Helper function for scan devs */
static void
set_funcandbars(struct ata_channel *ata, uint32_t barN, uint32_t barN1, uint32_t barN4) {
	if(barN == 0) {
		ata->ata_cmd_base = IDE_BAR0_DEFAULT;
		ata->read_port8 = ata_read8_io;
		ata->read_port16 = ata_read16_io;
		ata->write_port8 = ata_write8_io;
		ata->write_port16 = ata_write16_io;
	} else {
		if(barN & 1) {
			ata->ata_cmd_base = barN & (~0x1);
			ata->read_port8 = ata_read8_io;
			ata->read_port16 = ata_read16_io;
			ata->write_port8 = ata_write8_io;
			ata->write_port16 = ata_write16_io;
		} else {
			printf("Memory mapped ATA not yet supported (this is a fatal error)\n");
			return;
		}
	}
	if(barN1 == 0) {
		ata->ata_ctrl_base = IDE_BAR1_DEFAULT;
	} else {
		if(barN1 & 1) {
			ata->ata_ctrl_base = barN1 & (~0x1);
		} else {
			printf("Memory mapped ATA not yet supported\n");
		}
	}
	ata->ata_busm_base = barN4;
}

static void
set_other_stuff(struct ata_channel *ata, uint8_t devid, bdfo_t bdfo) {
	ata->devid = devid;
}

static void 
initialize_channel(struct ata_channel *ata) {
	uint8_t status = ata_read_status(ata);
	if (!(status & ATA_STS_BSY || status & ATA_STS_DRDY))
		ata->disabled = 1; 
	else {
		// It's safe to use write_port here (for now) because
		// status above called possible_update_dev()
		ata->write_port8(ata, ATA_CTRL_OFF, ATA_CTRL_nIEN);
		ata_do_identify(ata);
		uint32_t total_sectors = *(uint32_t*)&ata->identify_data->total_sectors;
		uint32_t logical_sector_size = *(uint32_t*)&ata->identify_data->logical_sector_size;
	}
}

void
ata_scan_devs() {
	int count_temp, count = 0;
	bdfo_t *temp = malloc(sizeof(bdfo_t) * PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC);
	pci_get_devs(PCI_CLASS_MASS_STORAGE, temp, &count);
	for(count_temp = 0; count_temp < count; count_temp++) {
		struct pci_header header;
		pci_read_header(temp[count_temp], &header);

		// We set up these devices like traditional IDE devices
		// TODO: is this appropriate for SATA?
		struct ata_channel *ata = alloc_ata_dev();
		set_funcandbars(ata, header.bar0, header.bar1, header.bar4);
		set_other_stuff(ata, ATA_DEV_DEV0, temp[count_temp]);
		initialize_channel(ata);

		// Set up second channel
		ata = alloc_ata_dev();
		set_funcandbars(ata, header.bar2, header.bar3, header.bar4 + 8);
		set_other_stuff(ata, ATA_DEV_DEV1, temp[count_temp]);
		initialize_channel(ata);
	}
	free(temp);
}

static void
possibly_update_dev(struct ata_channel *ata) {
	if (ata->idebus->current_dev != ata) {
		ata_write_dev(ata, ata->devid);
		ata->idebus->current_dev = ata;
		timed_delay(1);
	}
}

static uint8_t
ata_read_status(struct ata_channel *ata) {
	possibly_update_dev(ata);
	return (uint8_t)ata->read_port8(ata, ATA_STS_OFF);
}

static uint8_t
ata_wait_for(struct ata_channel *ata, uint8_t mask) {
	volatile uint8_t status;
	possibly_update_dev(ata);
	do {
		status = ata_read_status(ata);
	} while((status & mask) == 0);
}

static uint8_t
ata_wait_clear(struct ata_channel *ata, uint8_t mask) {
	uint8_t status;
	possibly_update_dev(ata);
	do {
		status = ata_read_status(ata);
	} while((status & mask));
}

static void
ata_write_dev(struct ata_channel *ata, uint8_t devdata) {
	ata->write_port8(ata, ATA_DEV_OFF, devdata);
}

static void
ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd) {
	possibly_update_dev(ata);
	if (cmd != ATA_CMD_DEVICE_RESET) {
		// wait until DRQ and BSY are 0 before submitting a new command
		uint8_t status ;
		do {
			status= ata_read_status(ata);
		} while(status & (ATA_STS_DRQ | ATA_STS_BSY));
	}
	ata->write_port8(ata, ATA_CMD_OFF, cmd);
}

static void
ata_dump_identity(struct ata_channel *ata) {
	int size = sizeof(struct ata_identify_data) / 2;
	int i;
	for(i = 0; i < size; i+=8) {
		printf("%04x %04x %04x %04x %04x %04x %04x %04x\n",
			((uint16_t *)ata->identify_data)[i],
			((uint16_t *)ata->identify_data)[i+1],
			((uint16_t *)ata->identify_data)[i+2],
			((uint16_t *)ata->identify_data)[i+3],
			((uint16_t *)ata->identify_data)[i+4],
			((uint16_t *)ata->identify_data)[i+5],
			((uint16_t *)ata->identify_data)[i+6],
			((uint16_t *)ata->identify_data)[i+7]);
	}
}

