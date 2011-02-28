#include <dev/block/block.h>
#include <fs/vfs.h>
#include <mm/malloc.h>
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
	else
		return (inode - 1) / super_block->s_inodes_per_group;
}

static block_id
group_to_blkid(struct ext2_super_block *super_block, uint64_t group)
{
	return 1 + (super_block->s_first_data_block) + (super_block->s_blocks_per_group * group);
}

static void
ext2_ls(struct vfs *fs)
{
	struct ext2_driver *ext2 = ((struct ext2_driver *)fs);
	struct block_device *bdev = fs->block_device;
	struct ext2_super_block *super_block = ext2->super_block;

	uint32_t inode_table_block = ext2->group_desc[inode_to_group(super_block, EXT2_ROOT_INO)].
					bg_inode_table;
	uint32_t table_blocks = ROUND_UP(super_block->s_inodes_per_group * super_block->s_inode_size, ext2->block_size) / ext2->block_size;

	void *inode_table= malloc(table_blocks * ext2->block_size);

	bdev->read_block(bdev, blkid_to_lba(ext2, inode_table_block),
		inode_table, table_blocks);
}
