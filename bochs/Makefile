#user settable tools
SHELL?=bash
SED?=sed
AWK?=awk
MKFS?=/sbin/mkfs.ext2
MKFS_FLAGS?=

#User settable FS size
FS_SIZE?=31457280
PART_START?=63

# media dependent variable
BLOCK_SIZE?=512
SECTORS_PER_TRACK?=63
HEADS_PER_CYLINDER?=16

#Derived info from media dependent stuff
SECTORS_PER_CYLINDER:=$(shell echo $(SECTORS_PER_TRACK)\*$(HEADS_PER_CYLINDER) | bc)
BYTES_PER_CYLINDER:=$(shell echo $(SECTORS_PER_CYLINDER)\*$(BLOCK_SIZE) | bc)
# Assume bootloader uses 1 track
SMALLBOOT_SIZE=$(shell echo $(SECTORS_PER_TRACK)\*$(BLOCK_SIZE) | bc)

# Do some math, what we actually want is a FS that is rounded up to a cylinder boundary
# boundary, and then subtract the smallboot size, which is assumed to be 1 track

# get number of blocks
FS_SECTORS_ORIG:=$(shell echo $(FS_SIZE)\/$(BLOCK_SIZE) | bc)

#I couldn't get the rounding to work in 1 step
# (fs_sectors + (sectors per cyclinder -) / sectors_per cylinder)
FS_CYLINDERS=$(shell echo "$(FS_SECTORS_ORIG)" | $(AWK) '{printf("%d\n",(($$0 + ($(SECTORS_PER_CYLINDER) - 1)) / $(SECTORS_PER_CYLINDER)))}')
FS_SECTORS=$(shell echo "$(FS_CYLINDERS)" | $(AWK) '{printf("%d\n",($$0 * $(SECTORS_PER_CYLINDER)))}')

#Assume smallboot is 1 track
FS_SECTORS_MINUS_SMALLBOOT=$(shell echo $(FS_SECTORS)\-$(SECTORS_PER_TRACK) | bc)
FS_SECTORS_MINUS_ONE=$(shell echo $(FS_SECTORS)\- 1 | bc)
USED_CYLINDERS=$(shell echo $(FS_SECTORS)\/$(SECTORS_PER_CYLINDER) | bc)
PART_TABLE_STRING=$(shell echo "partition_table 0x80 \
	$(shell ../tools/lba -h $(HEADS_PER_CYLINDER) -s $(SECTORS_PER_TRACK) -l $(PART_START)) 0x83 \
	$(shell ../tools/lba -h $(HEADS_PER_CYLINDER) -s $(SECTORS_PER_TRACK) -l $(FS_SECTORS_MINUS_ONE)) \
	$(PART_START) $(FS_SECTORS_MINUS_SMALLBOOT)")

all : bochsimg updatebochsrs
	@echo Created image with 
	@echo "    Block Size $(BLOCK_SIZE)"
	@echo "    Sectors per track $(SECTORS_PER_TRACK)"
	@echo "    Heads per sector $(HEADS_PER_CYLINDER)"
	@echo "    Filesystem blocks $(FS_SECTORS_MINUS_SMALLBOOT)"
	@echo "    Cylinders used $(USED_CYLINDERS)"

syms : ../kern/kernel
	./makesyms.bash ../kern/kernel symbols.txt

run :  bochsimg updatebochsrs
	bochs -q

force : all myfs.img

.PHONY : updatembr
updatembr :
	$(SED) 's/^partition_table1/$(PART_TABLE_STRING)/' ../boot/mbr/mbr_template.txt > ../boot/mbr/mbr.S

.PHONY : updatebochsrs
updatebochsrs :
	$(SED) -i.orig 's/^ata0-master.*/ata0-master: type=disk, mode=flat, path="bochsimg", cylinders=$(USED_CYLINDERS), heads=$(HEADS_PER_CYLINDER), spt=$(SECTORS_PER_TRACK)/' bochsrc.txt

../boot/bootimage: updatembr
	${MAKE} -C ../boot DEBUG=1
	
bochsimg: myfs.img ../boot/bootimage
	cp ../boot/bootimage ./bochsimg
	dd if=../boot/bootimage of=bochsimg ibs=$(SMALLBOOT_SIZE) conv=sync count=1  > /dev/null 2>&1
	cat myfs.img >> bochsimg

# put something like below in sudoers file to not need a password
#%wheel  ALL=(ALL)       NOPASSWD: /sbin/losetup, \
#        /sbin/mkfs.ext2, /dev/loop*, \
#        /bin/mount -o loop*, \
#        /bin/umount, \
#		/bin/chmod -R 777 hobosmnt/

myfs.img : ../kern/kernel
	-cp ../kern/kernel files/
	-dd if=/dev/zero of=temp.img bs=$(BLOCK_SIZE) count=$(FS_SECTORS_MINUS_SMALLBOOT) > /dev/null 2>&1
	-sudo losetup /dev/loop0 temp.img
	-sudo $(MKFS) $(MKFS_FLAGS) /dev/loop0  > /dev/null 2>&1
	-sudo mkdir -p hobosmnt/
	-sudo mount -o loop /dev/loop0 hobosmnt/
	-sudo chmod -R 777 hobosmnt/
	-cp files/* hobosmnt/
	-sudo umount hobosmnt/
	-sudo losetup -d /dev/loop0
	-mv temp.img myfs.img

.PHONY : clean
clean :
	rm bochsimg
	rm myfs.img
