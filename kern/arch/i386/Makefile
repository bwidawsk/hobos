DEFINES=-DKERNEL -Di386
CFLAGS=-ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -I../../../common/include $(DEFINES)

all : kernel 

header.o : header.S
	$(CC) $(CFLAGS) -DASM_FILE -c $< -o $@

kernel : header.o
	$(CC) -m32 $(CFLAGS) -c begin.c 
	$(LD) $(LDFLAGS) header.o begin.o -r -o arch.o

.PHONY: clean
clean :
	rm *.o
	-rm *.d
	rm kernel
