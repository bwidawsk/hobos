#ifndef __BLOCK_H__
#define __BLOCK_H__

struct block_device {
	struct device base;
	uint32_t block_size;
	int (*read_block)(struct block_device *dev, uint64_t lba, const void *buf, uint32_t count);
	int (*write_block)(struct block_device *dev, uint64_t lba, const void *buf, uint32_t count);
	// TODO: strategy like function
};

#define BLKDEV_FROM_DEV(dev) \
	((struct block_device *) (((void *)dev) - OFFSET_OF(struct block_device, base)))

#endif
