#include <dev/block/block.h>
#include "ext2.h"

uint8_t temp_storage[8192];

void
dump_bytes(uint8_t *buf, int count) {
	int i;
	for (i = 0; i < count; i++) {
		if (i && (i % 16 == 0))
			printf("\n");
		printf("%02x ", buf[i]);
	}
	printf("\n");
}

int
ext2_ls(struct block_device *dev, uint64_t lba_start)
{
	dev->read_block(dev, lba_start, temp_storage, 1);

	struct ext2_super_block *sup_block = (struct ext2_super_block *)temp_storage;
	if(sup_block->s_magic != EXT2_MAGIC) {
		printf("Not a valid superblock\n");
		dump_bytes(sup_block, 100);
		return -1;
	}
}
