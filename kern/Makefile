TARGET?=amd64
CC=x86_64-pc-linux-gnu-gcc
LD=x86_64-pc-linux-gnu-ld
#CFLAGS=-mcmodel=kernel -ansi -std=c99 -ffreestanding -nostdinc -nostartfiles -nodefaultlibs -nostdlib -Wall 
CFLAGS=-mcmodel=kernel -ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -Wall 
CFLAGS+=-DKERNEL -DAMD64 
CFLAGS+=-I$(PWD)/include -I$(PWD)/arch/$(TARGET)
CFLAGS+=-include $(PWD)/include/common.h

LDFLAGS?=

MEMORY_ALLOCATOR=DS_ALLOCATOR
CFLAGS+=-D$(MEMORY_ALLOCATOR)
export CC
export LD
export CFLAGS
export LDFLAGS
DIRS = include arch/$(TARGET) core dev lib mm
OBJS = arch/$(TARGET)/arch.a core/core.a dev/devices.a lib/lib.a mm/mm.a
OUTPUT = kernel

all : $(OUTPUT)

.PHONY : $(OUTPUT) 
$(OUTPUT) : include_clean
	cp -r arch/${TARGET}/include/ include/arch/
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -N -e start -T arch/${TARGET}/linker_script.ld -S $(OBJS) -o kernel

# autocleans if critical files change
include_clean : include/common.h 
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
	touch $@
	
.PHONY : clean
clean :
	-rm -rf include/arch
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
