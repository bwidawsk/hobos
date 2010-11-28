#include <stdint.h>
#include <dev/pci/pci_access.h>
#include <mm/malloc.h>
#include <mutex.h>
#include <timer.h> // timed_delay
#include "ata.h"

static void ata_dump_identity(struct ata_channel *ata);
static uint8_t ata_read_status(struct ata_channel *ata);
static void ata_wait_for(struct ata_channel *ata, uint8_t mask);
static void ata_wait_clear(struct ata_channel *ata, uint8_t mask);
static void ata_write_dev(struct ata_channel *ata, uint8_t devdata);
static void ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd);
static void possibly_update_dev(struct ata_channel *ata, uint8_t val);
static int ata_read_block(struct block_device *dev, uint64_t lba, void *buf, uint32_t count);
static int ata_write_block(struct block_device *dev, uint64_t lba, void *buf, uint32_t count);

extern uint8_t ata_read8_io(struct ata_channel *ata, uint8_t which);
extern uint16_t ata_read16_io(struct ata_channel *ata, uint8_t which);
extern void ata_write8_io(struct ata_channel *ata, uint8_t which, uint8_t data);
extern void ata_write16_io(struct ata_channel *ata, uint8_t which, uint16_t data); 

#define MAX_CHANNEL 5
static struct ata_channel ata_channels[MAX_CHANNEL + 1];
static struct ide_bus ide_busses[(MAX_CHANNEL + 1) / 2];
static int total_channel = 0;
static int scanned = 0;

#ifdef ATA1_SUPPORT
	// ATA1 devices need bits 7 and 5 set
	#define ATA_DEV_BITS 0xA0
#else
	#define ATA_DEV_BITS 0x0
#endif

#define IDE_BUS_LOCK(ata) 		mutex_acquire(ata->idebus->ide_bus_mtx)
#define IDE_BUS_RELEASE(ata)	mutex_release(ata->idebus->ide_bus_mtx)
#define ATA_CH_LOCK(ata)		mutex_acquire(ata->chan_mtx)
#define ATA_CH_RELEASE(ata)		mutex_release(ata->chan_mtx)

#define ATA_FROM_BLK(blk) ((struct ata_channel *)blk->pvt_data)

static struct ata_channel *
alloc_ata_dev() {
	KASSERT(total_channel < MAX_CHANNEL, ("not enough channel structures\n"));
	ata_channels[total_channel].id = total_channel;
	ata_channels[total_channel].idebus = &ide_busses[total_channel / 2];
	total_channel++;
	return &ata_channels[total_channel-1];
}

int 
ata_get_numdevs(void) {
	return total_channel;
}

void 
ata_init_blkdev(struct block_device *dev, int which) {
	struct ata_channel *ata = &ata_channels[which];
	dev->block_size = ata->sector_size;
	// these functions are defined at the bottom
	dev->read_block = ata_read_block;
	dev->write_block = ata_write_block;
	dev->pvt_data = ata;
}

/*
 * Basic function to do the pio reads, knows nothing of commands or
 * register state (except status)
 */
static int
ata_pio_read(struct ata_channel *ata, uint16_t *buf, uint64_t words) {
	volatile uint8_t status;
	while(words) {
		do {
			status = ata_read_status(ata);
			// TODO: we should sleep here?
		} while(!(status & ATA_STS_DRQ));

		if (!(status & ATA_STS_DRDY)) {
			printf("Fatal ATA error\n");
			return -1;
		}

		*buf = ata->read_port16(ata, ATA_DATA_REG);
		buf++;
		words--;
	}
	return 0;
}

static int
do_read_sectors_28lba(struct ata_channel *ata, uint32_t lba, uint16_t *buf, uint64_t sectors) {
	uint8_t dev_bits = ATA_DEV_BITS | ata->devid | ATA_DEV_LBA | (lba >> 24);
	possibly_update_dev(ata, dev_bits);
	ata->write_port8(ata, ATA_SECTCOUNT_REG, sectors);
	ata->write_port8(ata, ATA_LBAL_REG, lba & 0xFF);
	ata->write_port8(ata, ATA_LBAM_REG, (lba >> 8) & 0xFF);
	ata->write_port8(ata, ATA_LBAM_REG, (lba >> 16) & 0xFF);
	ata_write_cmd(ata, ATA_CMD_READ_SECTORS);
	ata_pio_read(ata, buf, (sectors * ata->sector_size) / 2);
}

int
ata_read_sectors(struct ata_channel *ata, void *buf, uint64_t lba, uint64_t count) {
	if (count > 256) {
		printf("Count > 256 sectors is not yet supported\n");
	}

	return 0;
}

static void
do_ata_identify(struct ata_channel *ata) {
	if (ata->identify_data != NULL) {
		printf("Warning, identify was already executed on ATA channel %d\n", ata->id);
	} else {
		ata->identify_data = malloc(sizeof(struct ata_identify_data));
	}

	possibly_update_dev(ata, ATA_DEV_BITS);

	ata->write_port8(ata, ATA_SECTCOUNT_REG, 0);
	ata->write_port8(ata, ATA_LBAL_REG, 0);
	ata->write_port8(ata, ATA_LBAM_REG, 0);
	ata->write_port8(ata, ATA_LBAH_REG, 0);

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
	if (ata->read_port8(ata, ATA_LBAM_REG) || ata->read_port8(ata, ATA_LBAH_REG))
		printf("HELP, device isn't ATA?");

	uint8_t error;
	while(1) {
		status = ata_read_status(ata);
		if (status & ATA_STS_DRQ)
			break;
		error = ata->read_port8(ata, ATA_ERROR_REG);
		// TODO: why do this?
		if (error & 1) {
			printf("Bad again\n");
			break;
		}
	}

	// size is in words, so divide by 2
	ata_pio_read(ata, (uint16_t *)ata->identify_data, sizeof(struct ata_identify_data) / 2);

}

/* Helper function for scan devs */
static void _INITSECTION_
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

static void _INITSECTION_
set_other_stuff(struct ata_channel *ata, uint8_t devid, bdfo_t bdfo) {
	ata->devid = devid;

	// TODO: sprintf
	MUTEX_INIT(ata->chan_mtx, "Ata channel mutex");
}

static void _INITSECTION_
initialize_channel(struct ata_channel *ata) {
	uint8_t status = ata_read_status(ata);
	if (!(status & ATA_STS_BSY || status & ATA_STS_DRDY))
		ata->disabled = 1; 
	else {
		// It's safe to use write_port here (for now) because
		// status above called possible_update_dev()
		ata->write_port8(ata, ATA_CTRL_REG, ATA_CTRL_nIEN);
		do_ata_identify(ata);
		//uint32_t total_sectors = *(uint32_t*)&ata->identify_data->total_sectors;
		ata->sector_size = *(uint32_t*)&ata->identify_data->logical_sector_size;
	}
}

static void _INITSECTION_
ata_setup_bus(struct ide_bus *idebus) {
	// TODO: sprintf
	MUTEX_INIT(idebus->ide_bus_mtx, "IDE bus mutex");
}

void _INITSECTION_
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

		// TODO: for now it's okay to setup the bus after the ata channels
		// but it's probably a good idea to do it before
		ata_setup_bus(ata->idebus);
	}
	free((void *)temp);
	scanned = 1;
}

/* val is all bits except the device ID */
static void
possibly_update_dev(struct ata_channel *ata, uint8_t val) {
	// adjust val to de device id specific
	val = ata->devid | (val & ATA_DEV_DEV0);

	if (ata->idebus->current_dev != ata)  {
		ata->devreg = val;
		ata_write_dev(ata, ata->devreg);
		ata->idebus->current_dev = ata;
		// wait 400 ns after changing device
		timed_delay(1);
	} else if (ata->devreg != val) {
		ata->devreg = val;
		ata_write_dev(ata, ata->devreg);
	}
}

static uint8_t
ata_read_status(struct ata_channel *ata) {
	possibly_update_dev(ata, ATA_DEV_BITS);
	return (uint8_t)ata->read_port8(ata, ATA_STS_REG);
}

static void
ata_wait_for(struct ata_channel *ata, uint8_t mask) {
	volatile uint8_t status;
	possibly_update_dev(ata, ATA_DEV_BITS);
	do {
		status = ata_read_status(ata);
	} while((status & mask) == 0);
}

static void
ata_wait_clear(struct ata_channel *ata, uint8_t mask) {
	uint8_t status;
	possibly_update_dev(ata, ATA_DEV_BITS);
	do {
		status = ata_read_status(ata);
	} while((status & mask));
}

static void
ata_write_dev(struct ata_channel *ata, uint8_t devdata) {
	ata->write_port8(ata, ATA_DEV_REG, devdata);
}

static void
ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd) {
	possibly_update_dev(ata, ATA_DEV_BITS);
	if (cmd != ATA_CMD_DEVICE_RESET) {
		// wait until DRQ and BSY are 0 before submitting a new command
		uint8_t status ;
		do {
			status= ata_read_status(ata);
		} while(status & (ATA_STS_DRQ | ATA_STS_BSY));
	}
	ata->write_port8(ata, ATA_CMD_REG, cmd);
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

static int 
ata_read_block(struct block_device *dev, uint64_t lba, void *buf, uint32_t count) {
	struct ata_channel *ata = ATA_FROM_BLK(dev);
	ATA_CH_LOCK(ata);
	ATA_CH_RELEASE(ata);

	return count;
}

static int 
ata_write_block(struct block_device *dev, uint64_t lba, void *buf, uint32_t count) {
	return 0;
}
