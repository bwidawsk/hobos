#include <stdint.h>
#include <md5.h>
#include "ata_block.h"

uint8_t temp_blks[256 * 512];

void do_ata_md5_test(int whichdev) {
	struct block_device blkdev;
	ata_init_blkdev(&blkdev, 0);
	struct ata_channel *ata = (struct ata_channel *)blkdev.pvt_data;
	KASSERT(ata->disabled == 0, ("badness"));
	printf("testing device with %d sectors of %d size\n", ata->num_sectors, ata->sector_size);

	int i = 0;
	blkdev.read_block(&blkdev, i, temp_blks, 255);
	printf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", 
		temp_blks[0],
		temp_blks[1],
		temp_blks[2],
		temp_blks[3],
		temp_blks[4],
		temp_blks[5],
		temp_blks[6],
		temp_blks[7],
		temp_blks[8],
		temp_blks[9],
		temp_blks[10],
		temp_blks[11],
		temp_blks[12],
		temp_blks[13],
		temp_blks[14],
		temp_blks[15]);
}
