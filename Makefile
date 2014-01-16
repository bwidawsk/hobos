BIOS=/usr/share/qemu/bios.bin
TERM=urxvt

# This represents the ideal setup with a tiled window manager
all:
	-rm mon ser
	-$(TERM) -e socat UNIX-LISTEN:ser - &
	-$(TERM) -e socat UNIX-LISTEN:mon - &
	-$(TERM) -e qemu-system-x86_64 bootimage/disk_image -S -s -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)
