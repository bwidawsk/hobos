#user settable tools
SHELL?=bash
SED?=sed
AWK?=awk
MKFS?=/sbin/mkfs.ext2
MKFS_FLAGS?=-F
TEMP_IMG?=temp.img

#User settable FS size
FS_SIZE?=31457280
PART_START?=63

# media dependent variable
BLOCK_SIZE?=512
SECTORS_PER_TRACK?=63
HEADS_PER_CYLINDER?=16

#Derived info from media dependent stuff
SECTORS_PER_CYLINDER:=$(shell echo $$[$(SECTORS_PER_TRACK)*$(HEADS_PER_CYLINDER)])
BYTES_PER_CYLINDER:=$(shell echo $$[$(SECTORS_PER_CYLINDER)*$(BLOCK_SIZE)])
# Assume bootloader uses 1 track
SMALLBOOT_SIZE=$(shell echo $$[$(SECTORS_PER_TRACK)*$(BLOCK_SIZE)])

# Do some math, what we actually want is a FS that is rounded up to a cylinder boundary
# boundary, and then subtract the smallboot size, which is assumed to be 1 track

# get number of blocks
FS_SECTORS_ORIG:=$(shell echo $$[$(FS_SIZE)/$(BLOCK_SIZE)])

#I couldn't get the rounding to work in 1 step
# (fs_sectors + (sectors per cyclinder -) / sectors_per cylinder)
FS_CYLINDERS=$(shell echo "$(FS_SECTORS_ORIG)" | $(AWK) '{printf("%d\n",(($$0 + ($(SECTORS_PER_CYLINDER) - 1)) / $(SECTORS_PER_CYLINDER)))}')
FS_SECTORS=$(shell echo "$(FS_CYLINDERS)" | $(AWK) '{printf("%d\n",($$0 * $(SECTORS_PER_CYLINDER)))}')

#Assume smallboot is 1 track
FS_SECTORS_MINUS_SMALLBOOT=$(shell echo $$[$(FS_SECTORS)-$(SECTORS_PER_TRACK)])
FS_SECTORS_MINUS_ONE=$(shell echo $$[$(FS_SECTORS)-1])
USED_CYLINDERS=$(shell echo $$[$(FS_SECTORS)/$(SECTORS_PER_CYLINDER)])
PART_TABLE_STRING=$(shell echo "partition_table 0x80 \
	$(shell ../userspace/tools/lba -h $(HEADS_PER_CYLINDER) -s $(SECTORS_PER_TRACK) -l $(PART_START)) 0x83 \
	$(shell ../userspace/tools/lba -h $(HEADS_PER_CYLINDER) -s $(SECTORS_PER_TRACK) -l $(FS_SECTORS_MINUS_ONE)) \
	$(PART_START) $(FS_SECTORS_MINUS_SMALLBOOT)")

#This is NOT multiprocess safe, but good enough...
LOOP_DEV=$(shell sudo losetup -f)

all : disk_image updatebochsrs ../userspace/tools/lba
	@echo Created image with 
	@echo "    Block Size $(BLOCK_SIZE)"
	@echo "    Sectors per track $(SECTORS_PER_TRACK)"
	@echo "    Heads per sector $(HEADS_PER_CYLINDER)"
	@echo "    Filesystem blocks $(FS_SECTORS_MINUS_SMALLBOOT)"
	@echo "    Cylinders used $(USED_CYLINDERS)"

syms : ../kern/kernel
	./makesyms.bash ../kern/kernel symbols.txt

force : all myfs.img

.PHONY : updatembr
updatembr :
	@$(SED) 's/^partition_table1/$(PART_TABLE_STRING)/' ../boot/mbr/mbr_template.txt > ../boot/mbr/mbr.S

.PHONY : updatebochsrs
updatebochsrs :
	@rm -f bochsrc.txt
	@touch bochsrc.txt
	@$(SED) -i.orig 's/^ata0-master.*/ata0-master: type=disk, mode=flat, path="disk_image", cylinders=$(USED_CYLINDERS), heads=$(HEADS_PER_CYLINDER), spt=$(SECTORS_PER_TRACK)/' bochsrc.txt

../boot/bootimage: updatembr
	${MAKE} -C ../boot DEBUG=1

disk_image: myfs.img ../boot/bootimage
	@cp ../boot/bootimage ./disk_image
	@dd if=../boot/bootimage of=disk_image ibs=$(SMALLBOOT_SIZE) conv=sync count=1  > /dev/null 2>&1
	@cat myfs.img >> disk_image

# put something like below in sudoers file to not need a password
#%wheel  ALL=(ALL)       NOPASSWD: /sbin/losetup, \
#        /sbin/mkfs.ext2, /dev/loop*, \
#        /bin/mount -o loop*, \
#        /bin/umount, \
#		/bin/chmod -R 777 hobosmnt/

myfs.img : ../kern/kernel
	@cp ../kern/kernel files/
	@dd if=/dev/zero of=$(TEMP_IMG) bs=$(BLOCK_SIZE) count=$(FS_SECTORS_MINUS_SMALLBOOT) > /dev/null 2>&1
	@sudo losetup $(LOOP_DEV) $(TEMP_IMG)
	@sudo $(MKFS) $(MKFS_FLAGS) $(LOOP_DEV)  > /dev/null 2>&1
	@sudo mkdir -p hobosmnt/
	@sudo mount -o loop $(LOOP_DEV)  hobosmnt/
	@sudo chmod -R 777 hobosmnt/
	@cp -r files/* hobosmnt/
	@sudo umount hobosmnt/
	@sudo losetup -d $(LOOP_DEV)
	@mv $(TEMP_IMG) myfs.img

check:
	../tools/ext2_tool

.PHONY : clean
clean :
	rm disk_image
	rm myfs.img
