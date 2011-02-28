#ifndef __VFS_H__
#define __VFS_H__

struct vfs {
	struct block_device *block_device;
	uint64_t start_lba;

	void (*ls)(struct vfs *fs);
};

struct vfs *ext2_init(struct block_device *dev, uint64_t lba_start);

#endif
