#include <init_funcs.h>
#include <device.h>
#include <dev/block/block.h>
#include "../vfs.h"

static struct vfs *rootfs;

struct vfs *
vfs_get(const char *path) {
	/* TODO rootfs is assumed */
	return rootfs;
}

INITFUNC_DECLARE(vfs_bootstrap, INITFUNC_DEVICE_VFS) {
	/* TODO First partitin is assumed to be root... fix this */
	struct device *dev = device_get(BLOCK_DEVICE, 0);
	/* TODO undo ext2 hardcoding */
	/*  Hack we need to read the partition table for this number, it's
	 * partition start */
	rootfs = ext2_init(BLKDEV_FROM_DEV(dev), 63);
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
