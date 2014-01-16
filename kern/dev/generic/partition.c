#include <master_boot_record.h>
#include <device.h>
#include <dev/block/block.h>
#include <init_funcs.h>
#include <mm/malloc.h>

void load_mbr(struct block_device *block_dev)
{
#define MAX_PARTITIONS 4
	struct master_boot_record temp_mbr; /* WARN: kernel stack size */
	struct primary_partition *partition;
	int i;

	uint8_t read_amt = sizeof(temp_mbr) / block_dev->block_size;
	if (!read_amt)
		read_amt++;

	block_dev->read_block(block_dev, 0, &temp_mbr, read_amt);
	if (temp_mbr.sig == 0xaa55)
		printf("Found a valid MBR for device\n");
	else
		return;

	block_dev->partition_table = malloc(sizeof(partition) * MAX_PARTITIONS);

	for (i = 0; i < MAX_PARTITIONS; i++) {
		partition = &temp_mbr.partitions[i];

		block_dev->partition_table[i] = malloc(sizeof(*partition));
		init_block_partition(block_dev->partition_table[i], partition->lba_first);

		if (partition->type != LINUX_PARTITION_TYPE) {
			continue;
		}
	}
}

INITFUNC_DECLARE(partition_check, INITFUNC_DEVICE_BLOCK_ANY) {
	int i = 0;
	struct device *dev;
	while(1) {
		dev = device_get(BLOCK_DEVICE, i++);
		if (dev == NULL)
			break;

		load_mbr(BLKDEV_FROM_DEV(dev));
	}
	printf("Done checking partitions\n");
}
