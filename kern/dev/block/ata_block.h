#ifndef __ATA_BLOCK_H__
#define __ATA_BLOCK_H__

#include "block.h"

struct ata_channel {
	struct block_device blkdev; /* parent structure */
	struct mutex *chan_mtx;
	struct ide_bus *idebus;
	uint8_t (*read_port8)(struct ata_channel *ata, uint8_t which);
	uint16_t (*read_port16)(struct ata_channel *ata, uint8_t which);
	void (*write_port8)(struct ata_channel *ata, uint8_t which, uint8_t data);
	void (*write_port16)(struct ata_channel *ata, uint8_t which, uint16_t data);
	int id;
	uint8_t devid;
	uint8_t devreg;
	uint64_t ata_cmd_base;
	uint64_t ata_ctrl_base;
	uint64_t ata_busm_base;
	struct ata_identify_data *identify_data;

	uint32_t sector_size;
	uint32_t num_sectors;
	int disabled;
	uint8_t bus;
	uint8_t dev;
	uint8_t func;
};

struct ide_bus {
	//Needed? struct ata_channel **channels
	struct ata_channel *current_dev;
	struct mutex *ide_bus_mtx;
};

#define ATA_FROM_BLK(blk) \
	((struct ata_channel *) (((void *)blk) - OFFSET_OF(struct ata_channel, blkdev)))

int ata_get_numdevs(void);
void ata_init_blkdev(struct block_device *dev, int which);

#endif
