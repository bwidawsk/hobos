DEFINES=-DKERNEL -Di386
CFLAGS=-ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -I../../../common/include $(DEFINES)

all : kernel

header.o: header.S
	$(CC) $(CFLAGS) -DASM_FILE -c $< -o $@

kernel: header.o
	$(CC) -m32 $(CFLAGS) -c begin.c
	$(LD) $(LDFLAGS) header.o begin.o -r -o arch.o

.PHONY: clean
clean:
	-@rm -f *.o kernel

SOURCES=early_putc.c libc_memfuncs.c libc_strfuncs.c freebsd_printf.c printf.c quicksort.c
OUTPUT=lib.a

include $(ROOTDIR)/hobo_kernel.mk
SOURCES=early_putc.c libc_memfuncs.c libc_strfuncs.c freebsd_printf.c printf.c quicksort.c
OUTPUT=lib.a
include $(ROOTDIR)/hobo_kernel.mk
