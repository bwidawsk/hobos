#include "bios.h"
#include "loader.h"
#include "small_libc.h"
#include "../../kern/fs/ext2/ext2.h"
#include "ext2_load.h"
#include "stdint.h"

static int block_size = 0;
static int sector_size = 0;
static uint32_t start_lba = 0;

// very basic memory allocator
static void *curptr = 0;

static volatile struct ext2_super_block superblock;

static unsigned int (*read_sector)(const void *, unsigned int, unsigned int[2]);

static struct ext2_inode get_root_inode();
static void get_inode(int inode_index, struct ext2_inode *ret_node);
static int get_inode_idx(struct ext2_inode *node, const char *path);

static int load_blocks(struct ext2_inode *inode, void *scratch);
static int load_nblocks(struct ext2_inode *inode, void *scratch, int n);

static int
block_to_lba(int block) {
        int sectors_per_block = block_size/sector_size;
        return (block * sectors_per_block) + start_lba;
}

/*
 *
 */
static int
read_block(const void *dest, int block, int count) {
	int sectors_per_block = block_size/sector_size;
	ASSERT(sectors_per_block != 0);
	ASSERT((block_size % sector_size) == 0);

	unsigned int lba[2];
	// TODO: this won't work for 2TB+ drives, must use a GUID partition table from EFI
	lba[1] = 0;
	lba[0] = block_to_lba(block);
	return read_sector(dest, count * sectors_per_block, lba);
}

void
initialize_block_loader(struct load_params *lparams) {

	read_sector = lparams->read_sector;
	sector_size = lparams->sector_size;
	// TODO: don't assume a partition in the future, pass in lparams
	start_lba = lparams->partitions->lba;
	curptr = (void *)(lparams->start_mb << 20);
#ifdef EXT2_DEBUG
	printf("ext2: %x choosen as memory base\n", curptr);
#endif

	// take a guess on block size, which is good enough for super block
	block_size = 1024;
	read_block(curptr, 0, 2);
	superblock = *(volatile struct ext2_super_block *)(curptr + 1024);
	block_size = 1024 << superblock.s_log_block_size;

#ifdef EXT2_DEBUG
	printf("Found superblock with block size %x\n", block_size);
#endif
}

#define GET_INODE_OR_DIE(__name, __inode) \
	do {\
		struct ext2_inode __root = get_root_inode(); \
		int __inode_index = get_inode_idx(&__root, __name); \
		if (__inode_index < 0) { \
			return 0; \
		} \
		get_inode(__inode_index, &__inode); \
	} while(0);

unsigned int
load_file_bytes(const char *name, void **addr, int nbytes) {

	ASSERT(curptr != 0);

	unsigned int num_blocks = nbytes / block_size;
	struct ext2_inode temp_node;

	if (num_blocks % block_size && nbytes > 0)
		num_blocks++;

	GET_INODE_OR_DIE(name, temp_node);
	if (nbytes <= 0)
		num_blocks = temp_node.i_blocks;

	if (*addr == 0) {
		*addr = curptr;
#ifdef EXT2_DEBUG
		printf("curptr moved from %x->%x\n",
		       curptr, curptr + num_blocks * block_size);
#endif
		curptr += num_blocks * block_size;
	} else {
#ifdef EXT2_DEBUG
		printf("load file to %x\n", *addr);
#endif
	}

	ASSERT(num_blocks > 0);
	load_nblocks(&temp_node, *addr, num_blocks);
	return num_blocks * block_size;
}

	// example of loading /foo/bar
	/*
	struct ext2_inode root = get_root_inode();
	int foo_index = get_inode_idx(&root, "foo");
	get_inode(foo_index, &foo_node)
	int bar_index = get_inode_idx(&foo_node, "bar");
	get_inode(bar_index, &bar_node);
	load_block(&bar_node, location);
	*/

static void
get_inode(int inode_index, struct ext2_inode *ret_node) {
#ifdef EXT2_DEBUG
	printf("get_inode %x\n", inode_index);
#endif
	int group = INODE_TO_GROUP(inode_index);

#ifdef EXT2_DEBUG
	printf("get_inode, group = %x\n", group);
	printf("get_inode, block = %x\n", GROUP_TO_BLOCK(group));
#endif

	read_block(curptr, GROUP_TO_BLOCK(group), 1);
	struct ext2_group_desc gdesc = *(volatile struct ext2_group_desc *)curptr;

	int inode_table_size = superblock.s_inodes_per_group * superblock.s_inode_size;
	int amt_to_read = inode_table_size / block_size;

#ifdef EXT2_DEBUG
	printf("get_inode, inode_table_size = %x (%x)\n", inode_table_size,
amt_to_read);
	printf("get_inode, inode table block = %x\n", gdesc.bg_inode_table);
#endif
	read_block(curptr, gdesc.bg_inode_table, amt_to_read);

	int index = INODE_TO_INDEX(inode_index);
	*ret_node = *INODE_IN_TABLE(curptr, index);
}

#if 0
static void
ls() {
	struct ext2_inode root = get_root_inode();
	get_inode_idx(&root, "garbanzo");
}
#endif

static struct ext2_inode
get_root_inode() {
	struct ext2_inode ret_node;
	get_inode(EXT2_ROOT_INO, &ret_node);
	return ret_node;
}

static int
load_blocks(struct ext2_inode *inode, void *scratch) {
	int blocks_loaded = 0;
	unsigned int blocks = inode->i_blocks;
	blocks_loaded = load_nblocks(inode, scratch, blocks);

	ASSERT(blocks_loaded == blocks);
	return blocks_loaded;
}

/**
 * TODO: get rid of constants
 */
static int
load_nblocks(struct ext2_inode *inode, void *scratch, int n) {
	int i;

	unsigned int *singly = 0;
	unsigned int *doubly = 0;

	unsigned int blocks = n;

	// This is where we'll store all the tables and other info
	void *indirect_blocks = scratch + blocks * block_size;

	/*
	 * These constants represent the logical blocks in each new level of the FS.
	 * It is dependent on block size, but if we assume a 1024 byte block, then as an example
	 * direct = block 0-11
	 * singly indirect = block 12 - 267
	 * doubly indirect = block 268 - 65803
	 * triply indirect = block 65804 - 16777215
	 */
	const int single_start = EXT2_IND_BLOCK;
	const int single_end = single_start + (block_size / 4) - 1;
	const int double_start = single_end + 1;
	const int double_end = double_start + (block_size / 4 * block_size / 4) - 1;
	const int triple_start = double_end + 1;
	//const int triple_end = triple_start + (block_size * block_size * block_size / 4 * 4 * 4) - 1;

	/* Read all the direct blocks of the inode */
	for ( i = 0; blocks && i < single_start; i++, blocks--) {
		read_block(scratch, inode->i_block[i], 1);
		scratch += block_size;
	}

	/* If there are no more blocks to read, exit */
	if (!blocks)
		return i;

	for ( i = single_start; blocks && i < double_start; i++, blocks--) {
		if (!singly) {
			singly = indirect_blocks;
			read_block(singly, inode->i_block[EXT2_IND_BLOCK], 1);
		}
		read_block(scratch, singly[i - single_start], 1);
		scratch += block_size;
	}

	/* If there are no more blocks to read, exit */
	if (!blocks)
		return i;

	for ( i = double_start; blocks && i < triple_start; i++, blocks--) {
		if (!doubly) {
			// doubly table starts one block after the singly table
			// we can safely wipe out the stored singly table
			//doubly = indirect_blocks + block_size; // if we want to preserve singly
			doubly = indirect_blocks;

			// read in the table, it should have a bunch of 4 byte entries
			read_block(doubly, inode->i_block[EXT2_DIND_BLOCK], 1);
			unsigned int *temp = doubly;
			int j;
			for (j = 0; j < block_size / 4; j++) {
				if (temp[j] == 0)
					break;

				/* NB double is unsigned int *, so math is in
				 * dwords */
				read_block(doubly + (((block_size) +
						    (block_size * j)) >> 2),
					   temp[j], 1);
			}
		}

		// We know have a linear table starting at doubly + block_size
		unsigned int *doubly_start = (void *)doubly + block_size;
		read_block(scratch, doubly_start[i - double_start], 1);
		scratch += block_size;
	}

	/* If there are no more blocks to read, exit */
	if (!blocks)
		return i;

	/*  TODO: implement triply indirect */
	printf("%x/%x (%x) blocks remain\n", blocks, n, triple_start);
	ASSERT(0);
	return i;
}

/*
 * Gets an inode index for the file that resides in the parent directory
 */
static int
get_inode_idx(struct ext2_inode *parent_dir, const char *file) {

	load_blocks(parent_dir, curptr);

	struct ext2_dir_entry_2 *dir = (struct ext2_dir_entry_2*) curptr;

	unsigned int addr2 = (unsigned int) dir;

	while (dir->file_type != EXT2_FT_UNKNOWN) {
		//printf("i_node %x (%s)\n", dir->inode, dir->name);
		if (!strcmp((char *)dir->name, file)) {
			return dir->inode;
		}
		addr2 += dir->rec_len;
		dir = (struct ext2_dir_entry_2 *)addr2;
	}

	return -1;
}
