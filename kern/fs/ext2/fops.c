#include <dev/block/block.h>
#include <fs/vfs.h>
#include <mm/malloc.h>
#include <lib.h>
#include "ext2.h"

struct ext2_driver {
	struct vfs base;
	struct ext2_super_block *super_block;
	uint32_t groups;
	struct ext2_group_desc *group_desc;

	uint32_t block_size;

	#define blkdev base.block_device
	#define startlba base.start_lba
};

typedef uint32_t block_id;

uint8_t temp_storage1[8192];
uint8_t temp_storage2[8192];

static void ext2_ls(struct vfs *fs);

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

static int
blkid_to_lba(struct ext2_driver *ext2, block_id fs_block)
{
	int sectors_per_block = ext2->block_size / ext2->blkdev->block_size;
	return (fs_block * sectors_per_block) + ext2->startlba;
}

struct vfs *
ext2_init(struct block_device *dev, uint64_t lba_start)
{
	struct ext2_super_block *super_block;
	struct ext2_driver *ext2;

	ext2 = malloc(sizeof(struct ext2_driver));
	if (!ext2)
		return NULL;

	ext2->base.start_lba = lba_start;
	ext2->base.block_device = dev;
	ext2->base.ls = ext2_ls;

	dev->read_block(dev, lba_start + (1024 / dev->block_size), temp_storage1, 1);

	super_block = (struct ext2_super_block *)temp_storage1;
	ext2->super_block = super_block;
	ext2->block_size = 1 << (super_block->s_log_block_size + 10);

	/* TODO: try to find another superblock? */
	if(super_block->s_magic != EXT2_MAGIC) {
		dump_bytes((uint8_t *)super_block, 100);
		return NULL;
	}

	ext2->groups = super_block->s_blocks_count / super_block->s_blocks_per_group;
	if (super_block->s_blocks_count % super_block->s_blocks_per_group)
		ext2->groups++;
	int blocks_for_gdesc_table = ROUND_UP((sizeof(struct ext2_group_desc) * ext2->groups), ext2->block_size) / ext2->block_size;
	/*
	 * The first block after the super block has the group descriptor table
	 * TODO: similar to superblock, we can find backups
	 */
	dev->read_block(dev, blkid_to_lba(ext2, 1 + super_block->s_first_data_block),
			temp_storage2, blocks_for_gdesc_table);
	ext2->group_desc = (struct ext2_group_desc *)temp_storage2;

	return &ext2->base;
}

static uint32_t
inode_to_group(struct ext2_super_block *super_block, block_id inode)
{
	if (inode == 0)
		return 0;
	else {
		/* inodes are numbered from 1 up, so -1 */
		return (inode - 1) / super_block->s_inodes_per_group;
	}
}

static block_id
group_to_blkid(struct ext2_super_block *super_block, uint64_t group)
{
	return 1 + (super_block->s_first_data_block) + (super_block->s_blocks_per_group * group);
}

#define EXT2_POINTERS_PER_BLOCK(ext2) (ext2->block_size / sizeof(uint32_t))

static uint32_t
do_load_indirect(struct ext2_driver *ext2, int blocks, const uint32_t block, const void *data)
{
	struct block_device *bdev = ext2->base.block_device;
	int i;
	void *copyout_data = (void *)data;

	if (blocks == 0)
		return 0;

	const void *indirect_block = malloc(ext2->block_size);
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, 1);
	for (i = 0; i < EXT2_POINTERS_PER_BLOCK(ext2) && blocks--; i++) {
		uint32_t blkptr = ((uint32_t *)indirect_block)[i];
		bdev->read_block(bdev, blkid_to_lba(ext2, blkptr), copyout_data, 1);
		copyout_data += ext2->block_size;
	}
	free(indirect_block);
	return i;
}

static uint32_t
do_load_dindirect(struct ext2_driver *ext2, int blocks, uint32_t block, const void *data)
{
	struct block_device *bdev = ext2->base.block_device;
	int i;
	void *copyout_data = (void *)data;
	uint32_t ret = 0;

	if (blocks == 0)
		return 0;

	const uint32_t *indirect_block = malloc(ext2->block_size);
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, 1);
	for (i = 0; i <  EXT2_POINTERS_PER_BLOCK(ext2) && blocks; i++) {
		int blocks_read = do_load_indirect(ext2, blocks, indirect_block[i], copyout_data);
		blocks -= blocks_read;
		ret += blocks_read;
		copyout_data += (ext2->block_size * blocks_read);
	}

	free(indirect_block);
	return ret;
}

static uint32_t
do_load_tinderect(struct ext2_driver *ext2, int blocks, uint32_t block, const void *data)
{
	struct block_device *bdev = ext2->base.block_device;
	int i;
	void *copyout_data = (void *)data;
	uint32_t ret = 0;

	if (blocks == 0)
		return 0;

	const uint32_t *indirect_block = malloc(ext2->block_size);
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, 1);
	for (i = 0; i <  EXT2_POINTERS_PER_BLOCK(ext2) && blocks; i++) {
		int blocks_read = do_load_dindirect(ext2, blocks, indirect_block[i], copyout_data);
		blocks -= blocks_read;
		ret += blocks_read;
		copyout_data += (ext2->block_size * blocks_read);
	}

	free(indirect_block);
	return ret;
}

/*
 * Load blocks for an inode into data
 */
static uint32_t
load_blocks(struct ext2_driver *ext2, const struct ext2_inode *inode, const void *data)
{
	struct block_device *bdev = ext2->base.block_device;
	uint32_t blocks;
	int i;
	void *copyout_data = (void *)data;

	KASSERT(inode->i_size >= 0, ("unexpected inode size"));
	if (inode->i_size == 0)
		return 0;

	blocks = inode->i_size / ext2->block_size;
	if (!blocks)
		blocks++;

	int countdown_blocks = blocks;

	/* Read direct blocks */
	for (i = 0; i < EXT2_IND_BLOCK; i++) {
		bdev->read_block(bdev, blkid_to_lba(ext2, inode->i_block[i]), copyout_data, 1);
		copyout_data += ext2->block_size;
		countdown_blocks --;
		if (!countdown_blocks)
			break;
	}

#define LOAD_INDIRECT(block) \
	{ \
		uint32_t indirect_blocks = do_load_indirect(ext2, countdown_blocks, inode->i_block[EXT2_DIND_BLOCK], copyout_data); \
		copyout_data += (indirect_blocks * ext2->block_size); \
		countdown_blocks -= indirect_blocks; \
	}
	/* Read indirect blocks */
	LOAD_INDIRECT(inode->i_block[EXT2_IND_BLOCK]);
	LOAD_INDIRECT(inode->i_block[EXT2_DIND_BLOCK]);
	LOAD_INDIRECT(inode->i_block[EXT2_TIND_BLOCK]);

	KASSERT(countdown_blocks == 0, ("badbad"));

	return blocks - countdown_blocks + 1;
}

static struct ext2_inode
get_inode(struct ext2_driver *ext2, uint64_t inode) {
	struct ext2_super_block *super_block = ext2->super_block;
	struct block_device *bdev = ext2->base.block_device;

	uint32_t inode_table_block = ext2->group_desc[inode_to_group(super_block, inode)].bg_inode_table;
	uint32_t table_blocks = super_block->s_inodes_per_group * super_block->s_inode_size / ext2->block_size;
	if (!table_blocks)
		table_blocks++;

	void *inode_table= malloc(table_blocks * ext2->block_size);
	KASSERT(inode_table, ("fail"));

	bdev->read_block(bdev, blkid_to_lba(ext2, inode_table_block), inode_table, table_blocks);

	/* inode numbering starts from 1, so -1 */
	struct ext2_inode ret = ((struct ext2_inode *)inode_table)[inode - 1];

	free(inode_table);

	return ret;
}

static int
get_dentry_from_parent(struct ext2_driver *ext2, const struct ext2_inode *parent_dir, const char *file, struct ext2_dir_entry_2 *dentry) {
	KASSERT(parent_dir != NULL, ("parent dir was null"));
	int ret = 1;

	int dir_blocks = parent_dir->i_size / ext2->block_size;
	if (!dir_blocks)
		dir_blocks++;

	void *dir = malloc(ext2->block_size * dir_blocks);
	load_blocks(ext2, parent_dir, dir);
	struct ext2_dir_entry_2 *dir_entries = (struct ext2_dir_entry_2 *)dir;
	struct ext2_dir_entry_2 *temp = dir_entries;
	while (temp->file_type != EXT2_FT_UNKNOWN) {
		if(!strncmp(file, temp->name, temp->name_len)) {
			*dentry = *temp;
			ret = 0;
			goto out;
		}
		temp = ((void *)temp) + temp->rec_len;
	}

out:
	free(dir);
	return ret;
}

static int
get_inode_for_path(struct ext2_driver *ext2, const char *path, struct ext2_inode *inode) {
	char *begin = (char *)path, *end, *temp = (char *)path;
	struct ext2_inode parent = get_inode(ext2, EXT2_ROOT_INO);
	struct ext2_dir_entry_2 temp_dir;

again:
	while (*begin == '/') {
		begin++;
		temp++;
	}

	if (*begin == 0) {
		*inode = parent;
		return 0;
	}

	end = strchr(begin, '/');
	if (end == NULL) {
		get_dentry_from_parent(ext2, &parent, (const char *)begin, &temp_dir);
		*inode = get_inode(ext2, temp_dir.inode);
	} else {
		int ret;
		struct ext2_inode temp_inode;
		char temp_file[end - begin + 1]; /* +1 for null terminate */
		temp_file[end - begin] = 0;
		strncpy(temp_file, begin, end - begin);
		ret = get_dentry_from_parent(ext2, &parent, temp_file, &temp_dir);
		if (ret) {
			printf("couldn't find %s\n", temp_file);
		} else {
			if (temp_dir.file_type != EXT2_FT_DIR) {
				printf("%s is not a dir\n", temp_file);
				return -1;
			}
			begin = end;
			end = NULL;
			parent = get_inode(ext2, temp_dir.inode);
			goto again;
		}
	}

	return 0;
}

static void
ext2_ls(struct vfs *fs)
{
	struct ext2_inode inode;
	get_inode_for_path((struct ext2_driver *)fs, "/", &inode);
	get_inode_for_path((struct ext2_driver *)fs, "/random", &inode);
	get_inode_for_path((struct ext2_driver *)fs, "/testdir/pooo", &inode);
}

static void
ext2_ls2(struct vfs *fs)
{
	struct ext2_driver *ext2 = ((struct ext2_driver *)fs);
	struct block_device *bdev = fs->block_device;
	struct ext2_super_block *super_block = ext2->super_block;
	int i;

	struct ext2_inode root_inode = get_inode(ext2, EXT2_ROOT_INO);

	int dir_blocks = root_inode.i_size / ext2->block_size;
	if (!dir_blocks)
		dir_blocks++;

	void *dir = malloc(ext2->block_size * dir_blocks);

	load_blocks(ext2, &root_inode, dir);
	for (i = 0; i < dir_blocks; i++) {
		bdev->read_block(bdev, blkid_to_lba(ext2, root_inode.i_block[i]), dir + (i * ext2->block_size), 1);
	}

	struct ext2_dir_entry_2 *root_dir = (struct ext2_dir_entry_2 *)dir;
	struct ext2_dir_entry_2 *temp = root_dir;
	while (temp->file_type != EXT2_FT_UNKNOWN) {
		printf("%s\n", temp->name);
		temp = ((void *)temp) + temp->rec_len;
	}
}
