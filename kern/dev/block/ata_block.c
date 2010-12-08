#include <stdint.h>
#include <dev/pci/pci_access.h>
#include <mm/malloc.h>
#include <mutex.h>
#include <init_funcs.h>
#include "ata.h"

static void ata_dump_identity(struct ata_channel *ata);
static void ata_write_dev(struct ata_channel *ata, uint8_t devdata, const int skip_check);
static void ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd, const int skip_check);
static int possibly_update_dev(struct ata_channel *ata, uint8_t val);
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

#define ATA1_SUPPORT
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

#define SKIP_CHECK 1
#define MUST_CHECK 0
#define UPDATE_CHECK(update) ((updated) ? MUST_CHECK : SKIP_CHECK)

#ifdef ATA_TIMED_DELAY
#include <timer.h> // timed_delay
#define ATA_DELAY_400NS \
	timed_delay(1)
#else
	#define ATA_DELAY_400NS \
	ata_read_asr(ata); \
	ata_read_asr(ata); \
	ata_read_asr(ata); \
	ata_read_asr(ata)
#endif

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

#define ata_read_status ata_read_asr
static uint8_t
ata_read_asr(struct ata_channel *ata) {
	uint8_t ret = ata->read_port8(ata, ATA_ASTS_REG);
	if (ret & ATA_STS_BSY) {
		/* 5.2.2 -
		 * When the BSY bit is set to one, the other bits in this register shall not be used. 
		 */
		return ATA_STS_BSY;
	}

	return ret;
}

static uint8_t
ata_read_sr(struct ata_channel *ata) {
	uint8_t ret = ata->read_port8(ata, ATA_ASTS_REG);
	if (ret & ATA_STS_BSY) {
		/* 5.14.2 -
		 * When the BSY bit is set to one, the other bits in this register shall not be used. 
		 */
		return ATA_STS_BSY;
	}
	return ret;

}

static void
ata_write_cmd(struct ata_channel *ata, ata_cmd_t cmd, const int skip_check) {
	/* 5.3.2
	 * this register shall only be written when BSY and DRQ are both
	 * cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_CMD_REG, cmd);
		return;
	}
	#endif
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
ata_write_data_port(struct ata_channel *ata, uint16_t data) {

	/* 5.4.2
	 * This port shall be accessed for host DMA data transfers only when DMACK- and DMARQ are asserted.
	 */
	 // TODO: check DMACK and DMARQ?
	ata->write_port16(ata, ATA_DATA_REG, data);
}

static uint16_t
ata_read_data_port(struct ata_channel *ata) {

	/* 5.4.2
	 * This port shall be accessed for host DMA data transfers only when DMACK- and DMARQ are asserted.
	 */
	 // TODO: check DMACK and DMARQ?
	return ata->read_port16(ata, ATA_DATA_REG);
}

static void
ata_write_data(struct ata_channel *ata, uint16_t data, const int skip_check) {

	/* 5.5.2
	 * This register shall be accessed for host PIO data transfer only when DRQ is set to one and DMACK- is not
	 * asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port16(ata, ATA_DATA_REG, data);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(!(status & ATA_STS_DRQ));
	return ata->write_port16(ata, ATA_DATA_REG, data);
}

static uint16_t
ata_read_data(struct ata_channel *ata, const int skip_check) {

	/* 5.5.2
	 * This register shall be accessed for host PIO data transfer only when DRQ is set to one and DMACK- is not
	 * asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) 
		return ata->read_port16(ata, ATA_DATA_REG);
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(!(status & ATA_STS_DRQ));

	return ata->read_port16(ata, ATA_DATA_REG);
}

static void
ata_write_dev(struct ata_channel *ata, uint8_t devdata, const int skip_check) {
	/* 5.6.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_DEV_REG, devdata);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_DEV_REG, devdata);
}

/* val is all bits except the device ID */
static int
possibly_update_dev(struct ata_channel *ata, uint8_t val) {
	// adjust val to de device id specific
	val = ata->devid | (val & ~ATA_DEV_DEV0);

	if (ata->idebus->current_dev != ata)  {
		ata->devreg = val;
		ata_write_dev(ata, ata->devreg, MUST_CHECK);
		ata->idebus->current_dev = ata;
		// wait 400 ns after changing device
		ATA_DELAY_400NS;
		return 1;
	} else if (ata->devreg != val) {
		ata->devreg = val;
		ata_write_dev(ata, ata->devreg, MUST_CHECK);
		return 1;
	}
	return 0;
}

static void
ata_write_ctrl(struct ata_channel *ata, uint8_t data, const int skip_check) {
	/* 5.7.2
	 * This register shall only be written when DMACK- is not asserted.
	 */
	 ata->write_port8(ata, ATA_CTRL_REG, data);
}

/* TODO: not really safe to just read this see 5.8.2 */
static uint8_t
ata_read_err(struct ata_channel *ata) {
	return ata->read_port8(ata, ATA_ERROR_REG);
}

static uint8_t
ata_read_dev(struct ata_channel *ata, const int skip_check) {
	/* 5.6.2
	 * The contents of this register are valid only when BSY is cleared to zero.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) 
		return ata->read_port8(ata, ATA_DEV_REG);
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & ATA_STS_BSY);
	return ata->read_port8(ata, ATA_DEV_REG);
}

static void
ata_write_lbah(struct ata_channel *ata, uint8_t data, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_LBAH_REG, data);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_LBAH_REG, data);
}

static uint8_t
ata_read_lbah(struct ata_channel *ata, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		return ata->read_port8(ata, ATA_LBAH_REG);
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	return ata->read_port8(ata, ATA_LBAH_REG);
}

static void
ata_write_lbam(struct ata_channel *ata, uint8_t data, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_LBAM_REG, data);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_LBAM_REG, data);
}

static uint8_t
ata_read_lbam(struct ata_channel *ata, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		return ata->read_port8(ata, ATA_LBAM_REG);
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	return ata->read_port8(ata, ATA_LBAM_REG);
}

static void
ata_write_lbal(struct ata_channel *ata, uint8_t data, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_LBAL_REG, data);
		return;
	}
	#endif

	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_LBAL_REG, data);
}

static uint8_t
ata_read_lbal(struct ata_channel *ata, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		return ata->read_port8(ata, ATA_LBAL_REG);
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	return ata->read_port8(ata, ATA_LBAL_REG);
}


static void
ata_write_lba(struct ata_channel *ata, uint32_t lba, const int skip_check) {
	/* 5.10.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_LBAH_REG, (lba >> 16) & 0xFF);
		ata->write_port8(ata, ATA_LBAM_REG, (lba >> 8) & 0xFF);
		ata->write_port8(ata, ATA_LBAL_REG, (lba >> 0) &0xFF);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_LBAH_REG, (lba >> 16) & 0xFF);
	ata->write_port8(ata, ATA_LBAM_REG, (lba >> 8) & 0xFF);
	ata->write_port8(ata, ATA_LBAL_REG, (lba >> 0) &0xFF);
}

static void
ata_write_sectcount(struct ata_channel *ata, uint8_t data, const int skip_check) {
	/* 5.13.2
	 * This register shall be written only when both BSY and DRQ are cleared to zero and DMACK- is not asserted.
	 */
	#ifndef ATA_EXTRA_SAFE
	if (skip_check) {
		ata->write_port8(ata, ATA_SECTCOUNT_REG, data);
		return;
	}
	#endif
	uint8_t status;
	do {
		status = ata_read_status(ata);
	} while(status & (ATA_STS_BSY | ATA_STS_DRQ));
	ata->write_port8(ata, ATA_SECTCOUNT_REG, data);
}

// BELOW THIS POINT THERE SHOULD BE NO DIRECT READ OR WRITE PORTS

#define ATA_POLL_DRQ0 -10
static int
ata_status_wait_unbusy(struct ata_channel *ata) {
	uint8_t status;
	do {
		status = ata_read_status(ata);
		if (status & ATA_STS_DFSE) 
			return -3;
		if (status & ATA_STS_ERRCHK) {
			printf("Warning, error return %x\n", ata_read_err(ata));
			return -2;
		}
	} while(status & ATA_STS_BSY);
	
	if (!(status & ATA_STS_DRQ)) {
		return ATA_POLL_DRQ0;
	}

	return !(status & ATA_STS_DRDY);
}

/*
 * Basic function to do the pio reads, knows nothing of commands or
 * register state (except status)
 */
static int
ata_pio_read(struct ata_channel *ata, uint16_t *buf, uint64_t words) {
	while(words) {
		/* FACT CHECK:
		 * we don't need to check DRQ bit if we're reading in the same block 
		 */
		*buf = ata_read_data(ata, SKIP_CHECK);
		buf++;
		words--;
	}
	return 0;
}

static int
do_read_sectors_28lba(struct ata_channel *ata, uint16_t *buf, uint32_t lba, uint8_t sectors) {
	uint8_t dev_bits = ATA_DEV_BITS | ata->devid | ATA_DEV_LBA | (lba >> 24);
	uint16_t real_sectors = (sectors == 0) ? 256 : sectors;
	int updated;
	int i;
	updated = possibly_update_dev(ata, dev_bits);
	ata_write_sectcount(ata, sectors, UPDATE_CHECK(updated));
	// Checked above
	ata_write_lba(ata, lba, SKIP_CHECK);
	// If the device wasn't updated, we don't have to check
	ata_write_cmd(ata, ATA_CMD_READ_SECTORS, SKIP_CHECK);
	ATA_DELAY_400NS;
	int ret = ata_status_wait_unbusy(ata);
	if (ret != 0) {
		printf("FIXME %d!!!\n", ret);
		return 0;
	}

	for(i = 0; i < real_sectors; i++) {
		ata_pio_read(ata, &buf[i * ata->sector_size / 2], ata->sector_size / 2);
		/* When entering this state from the HPIOI2 state,
		the host shall wait one PIO transfer cycle time before reading the Status register. The wait may be
		accomplished by reading the Alternate Status register and ignoring the result.
		*/
		// discard
		ret = ata_read_asr(ata);

		ret = ata_status_wait_unbusy(ata);
		/* Wait unbusy normally tells us the status of drdy, which we don't care about for pio */

		if (ret == ATA_POLL_DRQ0) {
			if ( (i + 1) < real_sectors) {
				printf("ATA WARNING!!! unexpected end to PIO\n");
				return real_sectors - 1;
			}
		}
	}
	return real_sectors;
}

static void
do_ata_identify(struct ata_channel *ata) {
	if (ata->identify_data != NULL) {
		printf("Warning, identify was already executed on ATA channel %d\n", ata->id);
	} else {
		ata->identify_data = malloc(sizeof(struct ata_identify_data));
	}

	int updated = possibly_update_dev(ata, ATA_DEV_BITS);
	ata_write_sectcount(ata, 0, UPDATE_CHECK(updated));
	ata_write_lba(ata, 0, SKIP_CHECK);

	// Wait for DRDY to issue the identify command
	// spec says i should do this, but it doesn't seem to work properly
	//ata_wait_for(ata, ATA_STS_DRDY);


	// ata_write_lba asserted DRQ and BSDY
	ata_write_cmd(ata, ATA_CMD_IDENTIFY, SKIP_CHECK);
	int ret = ata_status_wait_unbusy(ata);
	if(ret != 0) {
		// DRDY should have been set
		printf("ATA channel %d, device %d does not exist\n",  ata->id, ata->devid >> 4);
		ata->disabled = 1;
		return;
	}

	// According to OSDev, we need to check LBAM and LBAH for some older packet devices
	if(ata_read_lbam(ata, SKIP_CHECK) || ata_read_lbah(ata, SKIP_CHECK)) {
		printf("HELP, device isn't ATA?");
		ata->disabled = 1;
		return;
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
		ata_write_ctrl(ata, ATA_CTRL_nIEN, SKIP_CHECK);
		do_ata_identify(ata);
		ata->num_sectors = *(uint32_t*)&ata->identify_data->total_sectors;
		ata->sector_size = *(uint32_t*)&ata->identify_data->logical_sector_size;
		if(ata->sector_size == 0) {
			printf("Warning sector size came up as 0, fix this!\n");
			ata->sector_size = 512;
		}
	}
}

static void _INITSECTION_
ata_setup_bus(struct ide_bus *idebus) {
	// TODO: sprintf
	MUTEX_INIT(idebus->ide_bus_mtx, "IDE bus mutex");
}

INITFUNC_DECLARE(ata_scan_devs, INITFUNC_DEVICE_EARLY) {
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
	printf("ATA scan completed\n");
	scanned = 1;
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
	if (count > 256) {
		printf("Count > 256 sectors is not yet supported\n");
	} else {
	//	printf("read %lld sectors starting at %lld\n", count, lba);
	}

	int ret = do_read_sectors_28lba(ata, buf, (uint32_t)lba, (uint8_t)count);

	ATA_CH_RELEASE(ata);

	return ret;
	return count;
}

static int 
ata_write_block(struct block_device *dev, uint64_t lba, void *buf, uint32_t count) {
	return 0;
}
