#ifndef __VFS_H__
#define __VFS_H__

struct vfs_inode {
	void *fs_inode;
};

/*
 * Currently only 1 partition is supported, so VFS need not do much
 */
struct vfs {
	struct block_device *block_device;
	uint64_t start_lba;

	int (*load_file)(struct vfs *fs, const char *file);
	int (*ls)(struct vfs *fs, const char *dir, struct vfs_inode **);
};

struct vfs *ext2_init(struct block_device *dev, uint64_t lba_start);

#endif
