TARGET?=i386
CFLAGS=-Wall -Werror -m32 -ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -I../../common/include -I../../common/include/${TARGET}
ASFLAGS=--32

CFLAGS+=-DBOCHS_INT13_WORKAROUND

ifdef DEBUG
CFLAGOPTS+=-DDEBUG -g -O0
else
CFLAGOPTS+=-Os
endif

OBJECTS = head.o bootstrap.o smallboot.o tramp.o \
	  small_libc.o ext2_load.o legacy_int.o elf_load32.o elf_load64.o

all : smallboot.img

bootstrap.o : bootstrap.c
	$(CC) $(CFLAGS) -Os -c $<

%.o : %.c elf_load_common.c
	$(CC) $(CFLAGS) $(CFLAGOPTS) -c $<

%.o : %.S
	$(AS) $(ASFLAGS) $< -o $@

smallboot.img : $(OBJECTS)
	$(LD) -N -e start -T smallboot.lds -S --oformat binary $(OBJECTS) -o $@
	$(LD) -n -N -e start -T smallboot.lds $(OBJECTS) -o $@.elf

smallboot.lst16 : smallboot.img
	objdump --disassemble -mi8086 $<.elf

smallboot.lst32 : smallboot.img
	objdump --disassemble -mi386 $<.elf

clean:
	-@rm -f $(OBJECTS) smallboot.img smallboot.img.elf
