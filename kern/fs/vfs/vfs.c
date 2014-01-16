#include <init_funcs.h>
#include <device.h>
#include <dev/block/block.h>
#include "../vfs.h"

/* First partition is assumed to be root. We can change this by using a kernel
 * commandline option, or just always force it to be 0. For simplicity do the
 * latter. */
#define ROOTFS_PARTITION 0
static struct vfs *rootfs;

struct vfs *
vfs_get(const char *path) {
	/* TODO rootfs is assumed */
	return rootfs;
}

INITFUNC_DECLARE(vfs_bootstrap, INITFUNC_DEVICE_VFS) {
	/* FIXME: It is assumed that the first block device is the device
	 * containing rootfs */
	struct device *dev = device_get(BLOCK_DEVICE, 0);
	struct block_device *bdev = BLKDEV_FROM_DEV(dev);

	/* TODO undo ext2 hardcoding */
	rootfs = ext2_init(bdev, bdev->partition_table[ROOTFS_PARTITION]->first_block);
}

#include <bs_commands.h>
static void *
vfs_ls(int argc, char *argv[]) {
	char *path = argv[1];
	struct vfs *fs = vfs_get(path);
	/* TODO can't be NULL */
	fs->ls(fs, path, NULL);
	return NULL;
}
void
vfs_help() {
	printf("ls\n");
}

BS_COMMAND_DECLARE(ls, vfs_ls, vfs_help);
