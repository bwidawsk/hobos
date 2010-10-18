TARGET?=i386
CFLAGS=-ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -I../../common/include -I../../common/include/${TARGET}

ifdef DEBUG
CFLAGOPTS+=-DDEBUG -g -O0
else
CFLAGOPTS+=-Os
endif

OBJECTS = head.o bootstrap.o smallboot.o tramp.o small_libc.o ext2_load.o legacy_int.o elf_load.o elf_load64.o

all : smallboot.img

bootstrap.o : bootstrap.c
	$(CC) $(CFLAGS) -Os -c $<

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGOPTS) -c $<

%.o : %.S
	$(AS) $(ASFLAGS) $< -o $@

smallboot.img : $(OBJECTS)
	$(LD) -N -e start -T smallboot.lds -S --oformat binary $(OBJECTS) -o $@
	$(LD) -n -N -e start -T smallboot.lds $(OBJECTS) -o $@.elf

smallboot.lst16 : smallboot.img
	objdump --disassemble -mi8086 $<.elf

clean:
	rm -f $(OBJECTS)
	rm -f smallboot.img
	rm -f smallboot.img.elf
