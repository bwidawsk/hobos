#include <device.h>
#include <dev/block/block.h>
#include <fs/vfs.h>
#include <mm/malloc.h>
#include <lib.h>
#include "ext2.h"

struct ext2_driver {
	struct vfs base;
	struct ext2_super_block *super_block;
	uint32_t groups;
//	struct ext2_group_desc *group_desc;
	int blocks_for_gdesc_table;

	uint32_t block_size;

	#define blkdev base.block_device
	#define startlba base.start_lba
};

typedef uint32_t block_id;

uint8_t temp_storage1[8192];
uint8_t temp_storage2[8192];

static int ext2_ls(struct vfs *fs,  const char *dir, struct vfs_inode **inodes);

static int
blkid_to_lba(struct ext2_driver *ext2, block_id fs_block)
{
	int sectors_per_block = ext2->block_size / ext2->blkdev->block_size;
	return (fs_block * sectors_per_block) + ext2->startlba;
}

static uint64_t
ext2_blocks_to_native(struct ext2_driver *ext2, uint32_t ext2_blocks)
{
	int sectors_per_block = ext2->block_size / ext2->blkdev->block_size;
	if (!sectors_per_block)
		sectors_per_block++;

	return sectors_per_block * ext2_blocks;
}

#define ONE_BLOCK ext2_blocks_to_native(ext2, 1)

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

	dev->read_block(dev, lba_start + (1024 / dev->block_size), temp_storage1, 2);

	super_block = (struct ext2_super_block *)temp_storage1;
	ext2->super_block = super_block;
	ext2->block_size = 1 << (super_block->s_log_block_size + 10);

	/* TODO: try to find another superblock? */
	if(super_block->s_magic != EXT2_MAGIC) {
		DUMP_BYTES((uint8_t *)super_block, 100);
		return NULL;
	}

	ext2->groups = super_block->s_blocks_count / super_block->s_blocks_per_group;
	if (super_block->s_blocks_count % super_block->s_blocks_per_group) {
		KWARN(1, ("Unexpected group calculation"));
		ext2->groups++;
	}
	ext2->blocks_for_gdesc_table = ROUND_UP((sizeof(struct ext2_group_desc) * ext2->groups), ext2->block_size) / ext2->block_size;
	/*
	 * The first block after the super block has the group descriptor table
	 * TODO: similar to superblock, we can find backups
	 */
	dev->read_block(dev, blkid_to_lba(ext2, 1 + super_block->s_first_data_block),
			temp_storage2, ext2_blocks_to_native(ext2, ext2->blocks_for_gdesc_table));
//	ext2->group_desc = (struct ext2_group_desc *)temp_storage2;

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
group_to_block(struct ext2_super_block *super_block, uint64_t group)
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
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, ONE_BLOCK);
	for (i = 0; i < EXT2_POINTERS_PER_BLOCK(ext2) && blocks--; i++) {
		uint32_t blkptr = ((uint32_t *)indirect_block)[i];
		bdev->read_block(bdev, blkid_to_lba(ext2, blkptr), copyout_data, ONE_BLOCK);
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
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, ONE_BLOCK);
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
do_load_tindirect(struct ext2_driver *ext2, int blocks, uint32_t block, const void *data)
{
	struct block_device *bdev = ext2->base.block_device;
	int i;
	void *copyout_data = (void *)data;
	uint32_t ret = 0;

	if (blocks == 0)
		return 0;

	const uint32_t *indirect_block = malloc(ext2->block_size);
	bdev->read_block(bdev, blkid_to_lba(ext2, block), indirect_block, ONE_BLOCK);
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

	if (inode->i_size == 0) {
		printf("i_size was 0\n");
		return 0;
	}

	blocks = inode->i_size / ext2->block_size;
	if (!blocks)
		blocks++;

	int countdown_blocks = blocks;

	/* Read direct blocks */
	for (i = 0; i < EXT2_IND_BLOCK; i++) {
		bdev->read_block(bdev, blkid_to_lba(ext2, inode->i_block[i]), copyout_data, ONE_BLOCK);
		copyout_data += ext2->block_size;
		countdown_blocks--;
		if (!countdown_blocks)
			goto out;
	}

	uint32_t indirect_blocks = do_load_indirect(ext2, countdown_blocks, inode->i_block[EXT2_IND_BLOCK], copyout_data);
	copyout_data += (indirect_blocks * ext2->block_size);
	countdown_blocks -= indirect_blocks;
	if (!countdown_blocks)
		goto out;

	uint32_t dindirect_blocks = do_load_dindirect(ext2, countdown_blocks, inode->i_block[EXT2_DIND_BLOCK], copyout_data);
	copyout_data += (dindirect_blocks * ext2->block_size);
	countdown_blocks -= dindirect_blocks;
	if (!countdown_blocks)
		goto out;

	uint32_t tindirect_blocks = do_load_tindirect(ext2, countdown_blocks, inode->i_block[EXT2_TIND_BLOCK], copyout_data);
	copyout_data += (dindirect_blocks * ext2->block_size);
	countdown_blocks -= tindirect_blocks;
	if (!countdown_blocks)
		goto out;

	KASSERT(countdown_blocks == 0, ("badbad"));

out:
	return blocks - countdown_blocks + 1;
}

/* inode numbering starts from 1, so -1 */
static struct ext2_inode
get_inode(struct ext2_driver *ext2, uint64_t inode) {
	struct ext2_super_block *super_block = ext2->super_block;
	struct block_device *bdev = ext2->base.block_device;

	int group = inode_to_group(ext2->super_block, inode);
	int group_blk = group_to_block(ext2->super_block, group);
	struct ext2_group_desc *group_desc = malloc(ext2->block_size * ext2->blocks_for_gdesc_table);
	bdev->read_block(bdev, blkid_to_lba(ext2, group_blk), group_desc,
			 ext2_blocks_to_native(ext2, ext2->blocks_for_gdesc_table));

	uint32_t inode_table_block = group_desc[inode_to_group(super_block, inode)].bg_inode_table;
	free(group_desc);
	uint32_t table_blocks = super_block->s_inodes_per_group * super_block->s_inode_size / ext2->block_size;
	if (!table_blocks)
		table_blocks++;

	void *inode_table= malloc(table_blocks * ext2->block_size);
	KASSERT(inode_table, ("fail"));

	bdev->read_block(bdev, blkid_to_lba(ext2, inode_table_block), inode_table, ext2_blocks_to_native(ext2, table_blocks));

	/* inodes are 1 based, -1 */
	struct ext2_inode ret = ((struct ext2_inode *)inode_table)[(inode - 1) % ext2->super_block->s_inodes_per_group];

	free(inode_table);

	return ret;
}

static struct ext2_dir_entry_2 *
get_dentries_from_parent(struct ext2_driver *ext2,
			 const struct ext2_inode *parent_dir)
{
	int dir_blocks = parent_dir->i_size / ext2->block_size;
	if (!dir_blocks)
		dir_blocks++;
	void *dir = malloc(ext2->block_size * dir_blocks);
	KASSERT(dir, ("malloc fail"));
	load_blocks(ext2, parent_dir, dir);
	return dir;
}

#define FOREACH_DIR_IN_DENTRIES(dentry, dir_entries) \
	dentry = dir_entries; \
	for( ;dentry->file_type != EXT2_FT_UNKNOWN; dentry = ((void *)dentry) + dentry->rec_len)

static int
get_dentry_from_parent(struct ext2_driver *ext2,
		       const struct ext2_inode *parent_dir, const char *file,
		       struct ext2_dir_entry_2 *dentry)
{
	KASSERT(parent_dir != NULL, ("parent dir was null"));
	int ret = -1;

	struct ext2_dir_entry_2 *dir_entries = get_dentries_from_parent(ext2, parent_dir);
	struct ext2_dir_entry_2 *temp;

	FOREACH_DIR_IN_DENTRIES(temp, dir_entries) {
		if(!strncmp(file, temp->name, temp->name_len)) {
			*dentry = *temp;
			ret = 0;
			goto out;
		}
	}

out:
	free(dir_entries);
	return ret;
}

/*
 * Returns inode type
 */
static int
get_inode_for_path(struct ext2_driver *ext2, const char *path,
		   struct ext2_inode *inode) {
	char *begin = (char *)path, *end;
	struct ext2_inode parent = get_inode(ext2, EXT2_ROOT_INO);
	struct ext2_dir_entry_2 temp_dir;
	int ret;

	/* To keep code below symmetric, setup parent to be root */
	get_dentry_from_parent(ext2, &parent, ".", &temp_dir);

again:
	while (*begin == '/')
		begin++;

	if (*begin == 0) {
		/*
		 *  ends here on "/"
		 *  ends here on "/foo/"
		 */
		goto out;
	}

	end = strchr(begin, '/');
	if (end == NULL) {
		/*
		 * ends here on "/foo"
		 * ends here on "/foo/bar"
		 */
		ret = get_dentry_from_parent(ext2, &parent, (const char *)begin, &temp_dir);
		if (ret)
			return -1;
	} else {
		int ret;
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

out:
	*inode = get_inode(ext2, temp_dir.inode);
	return temp_dir.file_type;
}

static int
ext2_ls(struct vfs *fs,  const char *path, struct vfs_inode **inodes)
{
	struct ext2_inode inode;
	int ret;
	struct ext2_driver *ext2 = (struct ext2_driver *)fs;
	struct ext2_dir_entry_2 *dentry, *dentries;

	ret = get_inode_for_path(ext2, path, &inode);
	if (ret < 0) {
		printf("ls couldn't find %s\n", path);
		return 0;
	}

	KASSERT(ret == EXT2_FT_DIR, ("ls called on non directory"));
	dentries = get_dentries_from_parent(ext2, &inode);
	FOREACH_DIR_IN_DENTRIES(dentry, dentries) {
		/*  TODO: copy out to structure */
		printf("%s", dentry->name);
		if (dentry->file_type == EXT2_FT_DIR)
			printf("/");
		printf("\n");
	}
	free(dentries);
	return ret;
}
