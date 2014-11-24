#ifndef __VFS_H__
#define __VFS_H__

struct vfs_inode {
	void *fs_inode;
};

struct vfs;
struct vfs_ops {
	int (*probe)(struct vfs *fs);
	int (*load_file)(struct vfs *fs, const char *file, void *dest);
	int (*ls)(struct vfs *fs, const char *dir, struct vfs_inode **);
};

/*
 * Currently only 1 partition is supported, so VFS need not do much
 */
struct vfs {
	struct block_device *block_device;
	uint64_t start_lba;
	const struct vfs_ops *ops;
};

extern struct vfs *registered_fs[2];
extern int registered_fs_cnt;
static inline void
vfs_register_fs(struct vfs *vfs, const struct vfs_ops *ops)
{
	registered_fs[registered_fs_cnt++] = vfs;
	vfs->ops = ops;
}

static inline struct vfs *
vfs_get(const char *path)
{
	/* TODO: Need to deal with multiple FS */
	return registered_fs[0];
}

#endif
