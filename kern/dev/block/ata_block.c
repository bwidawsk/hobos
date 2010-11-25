#include <stdint.h>
#include <dev/pci/pci_access.h>
#include <mm/malloc.h>
#include "block.h"
#include "ata.h"

extern uint8_t ata_read8_io(struct ata_channel *ata, uint8_t which);
extern uint16_t ata_read16_io(struct ata_channel *ata, uint8_t which);
extern void ata_write8_io(struct ata_channel *ata, uint8_t which, uint8_t data);
extern void ata_write16_io(struct ata_channel *ata, uint8_t which, uint16_t data); 

/* Preallocate 6 channels */
static struct ata_channel ata_channels[6];
static int total_channel = 0;

uint8_t
ata_read_status(struct ata_channel *ata) {
	// TODO we don't need to delay the first time
	timed_delay(1);
	return (uint8_t)ata->read_port8(ata, ATA_STS_OFF);
}

void
ata_write_dev(struct ata_channel *ata, uint8_t devdata) {
	// wait until DRQ and BSY are 0 before submitting a new command
	uint8_t status ;
	do {
		status= ata_read_status(ata);
	} while(status & (ATA_STS_DRQ | ATA_STS_BSY));
	ata->write_port8(ata, ATA_DEV_OFF, devdata);
}
void
ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd) {
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
dump_identity(struct ata_channel *ata) {
	int size = sizeof(struct ata_identify_data) / 2;
	int i;
	for(i = 0; i < (size / 8); i+=8) {
		printf("%x %x %x %x %x %x %x %x\n",
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

int
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

void
ata_do_identify(struct ata_channel *ata) {
	if (ata->identify_data != NULL) {
		printf("Warning, identify was already executed on ATA channel %d\n", ata->id);
	} else {
		ata->identify_data = malloc(sizeof(struct ata_identify_data));
	}

	ata_write_dev(ata, ata->devid);

	// Wait for DRDY to issue the identify command
	while(!(ata_read_status(ata) & ATA_STS_DRDY));
	ata_write_cmd(ata, ATA_CMD_IDENTIFY);

	// size is in words, so divide by 2
	ata_pio_read(ata, (uint8_t *)ata->identify_data, sizeof(struct ata_identify_data) / 2);

	printf("%x\n", ata->identify_data->integrity);
}

void
ata_scan_devs() {
	int count_temp, count = 0;
	bdfo_t *temp = malloc(sizeof(bdfo_t) * PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC);
	pci_get_devs(PCI_CLASS_MASS_STORAGE, temp, &count);
	for(count_temp = 0; count_temp < count; count_temp++) {
		struct pci_header header;
		pci_read_header(temp[count_temp], &header);
		if(header.bar0 == 0) {
			ata_channels[total_channel].ata_cmd_base = IDE_BAR0_DEFAULT;
			ata_channels[total_channel].read_port8 = ata_read8_io;
			ata_channels[total_channel].read_port16 = ata_read16_io;
			ata_channels[total_channel].write_port8 = ata_write8_io;
			ata_channels[total_channel].write_port16 = ata_write16_io;
		} else {
			if(header.bar0 & 1) {
				ata_channels[total_channel].ata_cmd_base = header.bar0 & (~0x1);
				ata_channels[total_channel].read_port8 = ata_read8_io;
				ata_channels[total_channel].read_port16 = ata_read16_io;
				ata_channels[total_channel].write_port8 = ata_write8_io;
				ata_channels[total_channel].write_port16 = ata_write16_io;
			} else {
				printf("Memory mapped ATA not yet supported (this is a fatal error)\n");
				return;
			}
		}
		
		if(header.bar1 == 0) {
			ata_channels[total_channel].ata_ctrl_base = IDE_BAR1_DEFAULT;
		} else {
			if(header.bar1 & 1) {
				ata_channels[total_channel].ata_ctrl_base = header.bar1 & (~0x1);
			} else {
				printf("Memory mapped ATA not yet supported\n");
			}
		}

		ata_channels[total_channel].ata_busm_base = header.bar4;

		ata_channels[total_channel].id = total_channel;
		ata_channels[total_channel].devid = ATA_DEV_DEV0;
		ata_channels[total_channel].bus = PCI_BDFO_BUS(temp[count_temp]);
		ata_channels[total_channel].dev = PCI_BDFO_DEV(temp[count_temp]);
		ata_channels[total_channel].func = PCI_BDFO_FUNC(temp[count_temp]);

		total_channel++;

		if(header.bar2 == 0) {
			ata_channels[total_channel].ata_cmd_base = IDE_BAR2_DEFAULT;
			ata_channels[total_channel].read_port8 = ata_read8_io;
			ata_channels[total_channel].read_port16 = ata_read16_io;
			ata_channels[total_channel].write_port8 = ata_write8_io;
			ata_channels[total_channel].write_port16 = ata_write16_io;
		} else {
			if(header.bar2 & 1) {
				ata_channels[total_channel].ata_cmd_base = header.bar2 & (~0x1);
				ata_channels[total_channel].read_port8 = ata_read8_io;
				ata_channels[total_channel].read_port16 = ata_read16_io;
				ata_channels[total_channel].write_port8 = ata_write8_io;
				ata_channels[total_channel].write_port16 = ata_write16_io;
			} else {
				printf("Memory mapped ATA not yet supported (this is a fatal error)\n");
				return;
			}
		}
		
		if(header.bar3 == 0) {
			ata_channels[total_channel].ata_ctrl_base = IDE_BAR3_DEFAULT;
		} else {
			if(header.bar3 & 1) {
				ata_channels[total_channel].ata_ctrl_base = header.bar3 & (~0x1);
			} else {
				printf("Memory mapped ATA not yet supported\n");
			}
		}
		ata_channels[total_channel].ata_busm_base = header.bar4 + 8;
	
		ata_channels[total_channel].id = total_channel;
		ata_channels[total_channel].devid = ATA_DEV_DEV1;
		ata_channels[total_channel].bus = PCI_BDFO_BUS(temp[count_temp]);
		ata_channels[total_channel].dev = PCI_BDFO_DEV(temp[count_temp]);
		ata_channels[total_channel].func = PCI_BDFO_FUNC(temp[count_temp]);

		total_channel++;

		#ifdef ATA_DEBUG_BARS
		printf("bar0 %p\n", header.bar0);
		printf("bar1 %p\n", header.bar1);
		printf("bar2 %p\n", header.bar2);
		printf("bar3 %p\n", header.bar3);
		printf("bar4 %p\n", header.bar4);
		printf("bar5 %p\n", header.bar5);
		#endif
	}

	int i;
	for(i = 0; i < total_channel; i++) {
		uint8_t status = ata_read_status(&ata_channels[i]);
		// If the drive is not busy, and not ready, assume it's not present
		// TODO: is this a valid assumption?
		if (!(status & ATA_STS_BSY || status & ATA_STS_DRDY))
			ata_channels[i].disabled = 1;
		else {
			printf("found ata channel %d, status = %x, PCI = (%d, %d, %d)\n", i, status,
				ata_channels[i].bus, ata_channels[i].dev, ata_channels[i].func);
			ata_do_identify(&ata_channels[i]);
			dump_identity(&ata_channels[i]);
		}
	}
}
