BIOS=/usr/share/qemu/bios.bin
TERM=urxvt
FS_DEPS=$(wildcard bootimage/files/*)

# This represents the ideal setup with a tiled window manager
all: boot/bootimage
	-rm mon ser
	$(TERM) -title "HOBOS SERIAL PORT" -e socat UNIX-LISTEN:ser - &
	$(TERM) -title "HOBOS QEMU MONITOR" -e socat UNIX-LISTEN:mon - &
	sleep 1
	$(TERM) -e qemu-system-x86_64 bootimage/disk_image -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)

debug: boot/bootimage
	-rm mon ser
	$(TERM) -title "HOBOS SERIAL PORT" -e socat UNIX-LISTEN:ser - &
	$(TERM) -title "HOBOS QEMU MONITOR" -e socat UNIX-LISTEN:mon - &
	sleep 5 && $(TERM) -title "HOBOS GDB" -cd $(CURDIR)/kern -e gdb --command=debug_kernel.gdb kernel &
	sleep 1
	$(TERM) -e qemu-system-x86_64 bootimage/disk_image -s -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)
	#$(TERM) -e qemu-system-x86_64 bootimage/disk_image -S -s -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)

boot/bootimage: kern/kernel $(FS_DEPS)
	echo $?
	$(MAKE) -C bootimage/


.PHONY: all debug boot/bootimage
