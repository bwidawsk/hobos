#ifndef __BLOCK_H__
#define __BLOCK_H__

struct block_device {
	uint32_t block_size;
	int (*read_block)(struct block_device *dev, uint64_t lba, const void *buf, uint32_t count);
	int (*write_block)(struct block_device *dev, uint64_t lba, const void *buf, uint32_t count);
	void *pvt_data;
	// TODO: strategy like function
};

#endif
