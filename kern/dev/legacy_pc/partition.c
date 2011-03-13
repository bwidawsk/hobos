#include <master_boot_record.h>
#include <dev/block/block.h>
#include <device.h>
#include <init_funcs.h>

struct master_boot_record temp_mbr;

void load_mbr(struct block_device *block_dev)
{
	uint8_t read_amt = sizeof(temp_mbr) / block_dev->block_size;
	if (!read_amt)
		read_amt++;

	block_dev->read_block(block_dev, 0, &temp_mbr, read_amt);
	if (temp_mbr.sig == 0xaa55) {
		printf("Found a valid MBR for device\n");
	}
}

INITFUNC_DECLARE(partition_check, INITFUNC_DEVICE_BLOCK_ANY) {
	int i = 0;
	struct device *dev;
	while(1) {
		dev = device_get(BLOCK_DEVICE, i++);
		if (dev == NULL)
			break;

		load_mbr((struct block_device *)dev->pvt);
	}
	printf("Done checking partitions\n");
}
