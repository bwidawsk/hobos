#include <device.h>
#include <md5.h>
#include "ata_block.h"

uint8_t temp_blks[256 * 512] _INITSECTION_;

#define MD5_BLOCKS_PER_ATA_SECTOR (512 / MD5_BLOCK_SIZE)

/**
 * do_ata_md5_test - Reads all blocks on a gven device, and calculate the md5.
 *
 * @whichdev: The device (from 0-x) to operate on
 */
void do_ata_md5_test(int whichdev) {
	struct block_device blkdev;
	struct device *dev = device_get(BLOCK_DEVICE, 0);
	blkdev = *(struct block_device *)dev->pvt;
	struct ata_channel *ata = ATA_FROM_BLK(&blkdev);
	KASSERT(ata->disabled == 0, ("badness"));

	int test_sectors = ata->num_sectors;

	printf("ATA MD5 testing %d sectors of %d size\n", test_sectors, ata->sector_size);

	printf("* 256 sector\n"
	       "+  16 setort\n"
	       "-   1 sector\n");

	struct md5_context ctx;
	init_md5_ctx(&ctx, NULL, ata->sector_size * test_sectors);
	int i = 0, j;

	/* Do 256 ata blocks at a time since it goes faster in block IO */
	for(i = 0; i < test_sectors - 257; i+=256)  {
		blkdev.read_block(&blkdev, i, temp_blks, 256);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR * 256; j++)  {
			ctx.curptr = &temp_blks[j * MD5_BLOCK_SIZE];
			md5_hash_block(&ctx);
		}
		printf("*");
	}

	/* Do 16 ata blocks at a time since it goes faster in block IO */
	for( ;i < test_sectors - 17; i+=16)  {
		blkdev.read_block(&blkdev, i, temp_blks, 16);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR * 16; j++)  {
			ctx.curptr = &temp_blks[j * MD5_BLOCK_SIZE];
			md5_hash_block(&ctx);
		}
		printf("+");
	}

	/* Do a block at a time until the last block */
	for(; i < test_sectors - 1; i++)  {
		blkdev.read_block(&blkdev, i, temp_blks, 1);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR; j++)  {
			ctx.curptr = &temp_blks[j * MD5_BLOCK_SIZE];
			md5_hash_block(&ctx);
		}
		printf("-");
	}

	/* Do the last block and pad*/
	blkdev.read_block(&blkdev, i, temp_blks, 1);
	// Normally we'd do MD5_BLOCKS_PER_ATA_SECTOR - 1, but since we know our
	// size is divisible by 64, we must add an extra
	for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR; j++)  {
		ctx.curptr = &temp_blks[j * MD5_BLOCK_SIZE];
		md5_hash_block(&ctx);
	}

	ctx.curptr = &temp_blks[j * MD5_BLOCK_SIZE];
	int ret = pad_block(&ctx);
	md5_hash_block(&ctx);
	if (ret) {
		ctx.curptr += MD5_BLOCK_SIZE;
		md5_hash_block(&ctx);
	}
	printf("\nDevice %d MD5:", whichdev);
	display_md5hash(&ctx);
	printf("\n");
}
