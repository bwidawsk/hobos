#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <ext2.h>

#define GROUP_BLOCK(addr) \
	((volatile struct ext2_group_desc*) (addr))

#define BLOCK_X(x) \
	(crap + (block_size * x))

/*  Group block descriptor to real block # */
#define GROUP_TO_BLOCK(x) \
	(1 + (superblock->s_first_data_block) + (superblock->s_blocks_per_group * x))

#define INODE_TO_GROUP(x) \
	((x - 1) / superblock->s_inodes_per_group)

#define INODE_TO_INDEX(x) \
	((x - 1) % superblock->s_inodes_per_group)

#define INODE_IN_TABLE(table_addr, x) \
	((volatile struct ext2_inode *)((table_addr + (superblock->s_inode_size) * x)))

int
main(int argc, char *argv[]) {
	int i,j;
	int fd = open(argv[1], 0);
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

	struct ext2_super_block *superblock = (struct ext2_super_block *) (crap);

	if (superblock->s_magic == EXT2_MAGIC) {
		printf("this looks like an ext2 superblock\n");
	}
	int nblocks = superblock->s_blocks_count;
	int block_size = 1024 * (1 << superblock->s_log_block_size);
	printf("Number of blocks = %d\n", nblocks);
	printf("Number of blocks per group = %d\n", superblock->s_blocks_per_group);
	printf("Number of block groups %d-%f\n", superblock->s_inodes_count / superblock->s_inodes_per_group, ceil((float)nblocks / (float)superblock->s_blocks_per_group));
	printf("total size is = %d (bs=%d)\n", nblocks * block_size, block_size);
	printf("first useful block #%d\n", superblock->s_first_data_block);
	uint32_t first = superblock->s_first_data_block;
	/* try to map the actual size */
	crap -=0x400;
	crap = mremap(crap, sizeof(struct ext2_super_block), nblocks * block_size, MREMAP_MAYMOVE);
	if (crap == MAP_FAILED) {
		perror("mremap");
		exit(0);
	}
	superblock = BLOCK_X(first);
	///crap -= 0x400;
	struct ext2_group_desc *group = (struct ext2_group_desc*)
					BLOCK_X(GROUP_TO_BLOCK(INODE_TO_GROUP(2)));
	uint32_t table_block = group->bg_inode_table;
	printf("inode table, block #%d\n", table_block);

	int index = INODE_TO_INDEX(EXT2_ROOT_INO);

	struct ext2_inode *root_node = INODE_IN_TABLE(BLOCK_X(table_block), index);

	for(i=0; i < EXT2_N_BLOCKS; i++) {
		printf("data = %d\n", root_node->i_block[i]);
	}
	printf("\n");

	printf("root i_blocks = %x\n", root_node->i_blocks);
	printf("root i_flag = %x\n", root_node->i_flags);

	struct ext2_dir_entry_2 *root_dir = (struct ext2_dir_entry_2*) (crap + (root_node->i_block[0] * block_size));
	void *addr = root_dir;
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
