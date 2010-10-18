#include <stdint.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "ext2.h"

int 
main(int argc, char *argv[]) {
	int i,j;
	int fd = open("../bochs/myfs.img", 0);
	if (fd == -1) {
		perror("open");
		exit(0);
	}

	void *crap = mmap(0, sizeof(struct ext2_super_block), PROT_READ, MAP_PRIVATE, fd, 0);
	crap+=0x400;
	if (crap == MAP_FAILED) {
		perror("mmap");
		exit(0);
	}
	
	struct ext2_super_block *sup_block = (struct ext2_super_block *) (crap);

	if (sup_block->s_magic == EXT2_MAGIC) {
		printf("this looks like an ext2 superblock\n");
	}
	int nblocks = sup_block->s_blocks_count;
	int block_size = 1024 * (1 << sup_block->s_log_block_size);
	printf("total size is = %d (bs=%d)\n", nblocks * block_size, block_size);
	/* try to map the actual size */
	crap -=0x400;
	crap = mremap(crap, sizeof(struct ext2_super_block), nblocks * block_size, MREMAP_MAYMOVE);
	if (crap == MAP_FAILED) {
		perror("mremap");
		exit(0);
	}
	///crap -= 0x400;
	struct ext2_group_desc *group = (struct ext2_group_desc*) (crap + (2*block_size));
	printf("inode table, block #%d\n", group->bg_inode_table);

	// crap is actually pointing to block 1, so our math is a little off
	// inode #2 should be root
		struct ext2_inode *root_node = (struct ext2_inode *)((crap + (8* block_size)) + (sizeof(struct ext2_inode) * 1));
		for(i=0; i < EXT2_N_BLOCKS; i++) {
			printf("data = %d\n", root_node->i_block[i]);
		}
		printf("\n");

		printf("root i_blocks = %x\n", root_node->i_blocks);
		printf("root i_flag = %x\n", root_node->i_flags);

	struct ext2_dir_entry_2 *root_dir = (struct ext2_dir_entry_2*) (crap + (root_node->i_block[0] * block_size));
	uint32_t addr = root_dir;
	struct ext2_dir_entry_2 *temp = addr;
	while (temp->file_type != EXT2_FT_UNKNOWN) {
		if (!strcmp(temp->name, "hload")) {
			
		}
		printf("inode %x (%s)\n", temp->inode, temp->name);
		addr+=temp->rec_len;	
		temp = addr;
	}
	
	return 0;
}
