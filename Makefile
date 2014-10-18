BIOS=/usr/share/qemu/bios.bin
TERM=urxvt

# This represents the ideal setup with a tiled window manager
all:
	-rm mon ser
	$(TERM) -title "HOBOS SERIAL PORT" -e socat UNIX-LISTEN:ser - &
	$(TERM) -title "HOBOS QEMU MONITOR" -e socat UNIX-LISTEN:mon - &
	sleep 1
	$(TERM) -e qemu-system-x86_64 bootimage/disk_image -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)

debug:
	-rm mon ser
	$(TERM) -title "HOBOS SERIAL PORT" -e socat UNIX-LISTEN:ser - &
	$(TERM) -title "HOBOS QEMU MONITOR" -e socat UNIX-LISTEN:mon - &
	sleep 5 && $(TERM) -title "HOBOS GDB" -cd $(CURDIR)/kern -e gdb --command=debug_kernel.gdb kernel &
	sleep 1
	$(TERM) -e qemu-system-x86_64 bootimage/disk_image -s -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)
	#$(TERM) -e qemu-system-x86_64 bootimage/disk_image -S -s -serial unix:ser -monitor unix:mon -debugcon stdio -bios $(BIOS)
