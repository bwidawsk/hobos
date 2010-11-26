#ifndef __ATA_BLOCK_H__
#define __ATA_BLOCK_H__

struct ata_channel {
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

	uint32_t block_size;
	uint64_t size;
	int disabled;
	uint8_t bus;
	uint8_t dev;
	uint8_t func;
};

struct ide_bus {
	//Needed? struct ata_channel **
	struct ata_channel *current_dev;
	struct mutex *ide_bus_mtx;
};

void ata_scan_devs(void);
int ata_get_numdevs(void);
struct ata_channel *ata_get_channel(int which);
int ata_read_sectors(struct ata_channel *ata, void *buf, uint64_t lba, uint64_t count);

#endif
