SHELL=/bin/bash
# sectors per track
SPT?=63
# heads per cylinder
HPC?=16

FS_SIZE?=

bootimg:
	${MAKE} -C mbr
	${MAKE} -C smallboot DEBUG=1
	cat mbr/mbr.img > bootimage
	cat smallboot/smallboot.img >> bootimage

.PHONY : clean
clean :
	-@rm -f bootimage
	${MAKE} -C mbr clean
	${MAKE} -C smallboot DEBUG=1 clean

.PHONY: tags
tags:
	ctags -R .
	ctags -aR ../common
