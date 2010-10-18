#define GROUP_BLOCK(addr) \
	((volatile struct ext2_group_desc*) (addr))

#define BLOCK_X(x) \
	(base + (block_size * x))

/*  Group block descriptor to real block # */
#define GROUP_TO_BLOCK(x) \
	(1 + (superblock.s_first_data_block) + (superblock.s_blocks_per_group * x))

#define INODE_TO_GROUP(x) \
	((x - 1) / superblock.s_inodes_per_group)

#define INODE_TO_INDEX(x) \
	((x - 1) % superblock.s_inodes_per_group)

#define INODE_IN_TABLE(table_addr, x) \
	((volatile struct ext2_inode *)((table_addr + (superblock.s_inode_size) * x)))
