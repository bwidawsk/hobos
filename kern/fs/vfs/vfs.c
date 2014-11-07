#include <init_funcs.h>
#include <device.h>
#include <dev/block/block.h>
#include "../vfs.h"

/* TODO: linked list when available */
struct vfs *registered_fs[2];
int registered_fs_cnt = 0;

/* First partition is assumed to be root. We can change this by using a kernel
 * commandline option, or just always force it to be 0. For simplicity do the
 * latter. */
#define ROOTFS_PARTITION 0

INITFUNC_DECLARE(vfs_bootstrap, INITFUNC_DEVICE_VFS) {
	/* FIXME: It is assumed that the first block device is the device
	 * containing rootfs */
	struct device *dev = device_get(BLOCK_DEVICE, 0);
	struct block_device *bdev = BLKDEV_FROM_DEV(dev);
	int i;

	for (i = 0; i < 2; i++) {
		if (registered_fs[i]) {
			registered_fs[i]->block_device = bdev;
			registered_fs[i]->start_lba = bdev->partition_table[ROOTFS_PARTITION]->first_block;
			registered_fs[i]->ops->probe(registered_fs[i]);
		}
	}
}

#include <bs_commands.h>
static void *
vfs_ls(struct console_info *info, int argc, char *argv[]) {
	char *path = argv[1];
	struct vfs *fs = vfs_get(path);
	/* TODO can't be NULL */
	fs->ops->ls(fs, path, NULL);
	return NULL;
}

static void
vfs_help() {
	printf("ls\n");
}

BS_COMMAND_DECLARE(ls, vfs_ls, vfs_help);
