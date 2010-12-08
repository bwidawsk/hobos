TARGET?=amd64
CC=x86_64-pc-linux-gnu-gcc
LD=x86_64-pc-linux-gnu-ld
MAKEDEPEND = makedepend
export ROOTDIR=$(PWD)
export CC
export LD
export MAKEDEPEND

MEMORY_ALLOCATOR=DS_ALLOCATOR

# I'd like to have no builtins so that GCC isn't required, but can't avoid for now
#CFLAGS+=-fno-builtin
CFLAGS+=-mcmodel=kernel -ansi -std=c99 -ffreestanding -nostdinc -nostartfiles -nodefaultlibs -nostdlib -Wall 

DEFINES=-DKERNEL -DAMD64 -D$(MEMORY_ALLOCATOR)
CFLAGS+=$(DEFINES)

INCLUDES=-I$(PWD)/include -I$(PWD)/arch/$(TARGET) -I$(PWD) -I$(ROOTDIR)/../common/include -include $(PWD)/include/common.h
CFLAGS+=$(INCLUDES)

LDFLAGS?=
export CFLAGS
export LDFLAGS
export DEFINES
export INCLUDES

DIRS = include arch/$(TARGET) core dev lib mm
OBJS = arch/$(TARGET)/arch.a core/core.a dev/devices.a lib/lib.a mm/mm.a
OUTPUT = kernel

all : $(OUTPUT)

.PHONY : $(OUTPUT) 
$(OUTPUT) : include_clean
	./vers.sh
	-rm include/arch
	ln -sf $(PWD)/arch/${TARGET}/include/ $(PWD)/include/arch
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -N -e start -T arch/${TARGET}/linker_script.ld -S $(OBJS) -o kernel

# autocleans if critical files change
include_clean : include/common.h
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
	touch $@

.PHONY : tags
tags :
	ctags -R .

.PHONY : picocom
picocom :
	sudo picocom -b 115200 -i -r /dev/ttyp9 -c

help :
	@echo "Available targets: "
	@echo " all (default)"
	@echo " tags"
	@echo " picocom"
	@echo " clean"
	@echo "Variables which you may want to change: "
	@echo " CC default: $(CC)"
	@echo " LD default: $(LD)"
	@echo " CFLAGS default: $(CFLAGS)"
	@echo " LDFLAGS default: $(LDFLAGS)"


.PHONY : clean
clean :
	-rm core/vers.c
	-rm include/arch
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
