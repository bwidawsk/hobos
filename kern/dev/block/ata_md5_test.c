#include <device.h>
#include <md5.h>
#include "ata_block.h"
#include "block.h"

/* This should be dynamic... */
typedef char ata_block_t[512];
#define MD5_BLOCKS_PER_ATA_SECTOR (512 / MD5_BLOCK_SIZE)
md5_block_t temp_blks[MD5_BLOCKS_PER_ATA_SECTOR * 256];

#include <bs_commands.h>
/**
 * do_ata_md5_test - Reads all blocks on a gven device, and calculate the md5.
 *
 * @whichdev: The device (from 0-x) to operate on
 */
static void *
do_ata_md5_test(struct console_info *info, int argc, char *argv[]) {
	struct device *dev;
	struct block_device *blkdev;
	struct ata_channel *ata;
	struct md5_context ctx;
	int i=0, j;

	dev = device_get(BLOCK_DEVICE, 0);
	blkdev = BLKDEV_FROM_DEV(dev);
	ata = ATA_FROM_BLK(blkdev);
	KASSERT(ata->disabled == 0, ("Ata device (%d %d %d) is disabled"),
			ata->bus, ata->dev, ata->func);

	int test_sectors = ata->num_sectors;

	printf("ATA MD5 testing %d sectors of %d size\n", test_sectors, ata->sector_size);

	printf("* 256 sector\n"
	       "+  16 setort\n"
	       "-   1 sector\n");

	init_md5_ctx(&ctx, NULL, ata->sector_size * test_sectors);

	/* Do 256 ata blocks at a time since it goes faster in block IO */
	for(i = 0; i <= test_sectors - 256; i+=256)  {
		blkdev->read_block(blkdev, i, temp_blks, 256);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR * 256; j++)  {
			ctx.curptr = &temp_blks[j];
			md5_hash_block(&ctx);
		}
		printf("*");
	}

	/* Do 16 ata blocks at a time since it goes faster in block IO */
	for( ;i <= test_sectors - 16; i+=16)  {
		blkdev->read_block(blkdev, i, temp_blks, 16);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR * 16; j++)  {
			ctx.curptr = &temp_blks[j];
			md5_hash_block(&ctx);
		}
		printf("+");
	}

	/* Do a block at a time until the last block */
	for(; i <= test_sectors - 1; i++)  {
		blkdev->read_block(blkdev, i, temp_blks, 1);
		for(j = 0; j < MD5_BLOCKS_PER_ATA_SECTOR; j++)  {
			ctx.curptr = &temp_blks[j];
			md5_hash_block(&ctx);
		}
		printf("-");
	}

	int ret = pad_block(&ctx);
	md5_hash_block(&ctx);
	if (ret) {
		KWARN(1, ("Pad required, not expected, bailing\n"));
		return NULL;
	}

	printf("\n");
	display_md5hash(&ctx);
	printf("\n");

	return NULL;
}

static void
md5_dev_help() {
	printf("Does an md5 the first block device\n");
}

BS_COMMAND_DECLARE(md5_dev, do_ata_md5_test, md5_dev_help);
