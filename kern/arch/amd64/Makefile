SOURCES=begin.c mm_amd64.c atomic_amd64.c idt.c reset.c arch.c libc_memfuncs.c debug.c
OBJS=$(subst .c,.o,$(SOURCES)) idt_handlers.o
INCLUDES+=-I. -I../../ -I../ia_common -I../../../common/include -I../../../common/include/amd64
CFLAGS+=$(INCLUDES)

HEADEROBJS=header.o early_pages.o
HEADEROBJ_CFLAGS=-ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -DKERNEL -DAMD64 -mcmodel=kernel $(INCLUDES)

all: kernel

idt_handlers.o: idt_handlers.S ../ia_common/idt_common.h
	$(CC) $(INCLUDES) -DASM_FILE -c $< -o $@

header.o: header.S ../../../common/include/multiboot.h
	$(CC) $(HEADEROBJ_CFLAGS) -DASM_FILE -c $< -o $@

early_pages.o: early_pages.c
	$(CC) $(HEADEROBJ_CFLAGS) -c $< -o $@

kernel: $(OBJS) $(HEADEROBJS)
	$(LD) $(LDFLAGS)  $(OBJS) $(HEADEROBJS) -r -o arch.a

.PHONY: clean
clean :
	-@rm -f arch.a *.o

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
