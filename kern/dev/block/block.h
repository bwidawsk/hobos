#ifndef __BLOCK_H__
#define __BLOCK_H__

/* Thanks to life, there are multiple ways in which a partition can be defined,
 * legacy PC for instance uses one thing, GPT another. Since the details of these
 * are mostly uninteresting, we use an abstraction to define the fields we actually
 * care about
 */
struct block_partition {
	uint64_t first_block;
};

static inline void init_block_partition(struct block_partition *bpart,
										uint64_t start)
{
	bpart->first_block = start;
}

struct block_device {
	struct device base;
	uint32_t block_size;
	int (*read_block)(struct block_device *dev, uint64_t lba, const void *buf,
					  uint32_t count);
	int (*write_block)(struct block_device *dev, uint64_t lba, const void *buf,
					   uint32_t count);
	// TODO: strategy like function

	struct block_partition **partition_table;
};

#define BLKDEV_FROM_DEV(dev) \
	((struct block_device *) (((void *)dev) - OFFSET_OF(struct block_device, base)))

#endif
