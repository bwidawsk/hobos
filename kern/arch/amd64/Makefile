OUTPUT=arch.a
SOURCES=arch.c atomic_amd64.c begin.c debug.c idt.c libc_memfuncs.c mm_amd64.c reset.c
EARLY_OBJS=idt_handlers.o header.o early_pages.o
INCLUDES+=-I../ia_common -Iinclude
CFLAGS+=$(INCLUDES)

include $(ROOTDIR)/hobo_kernel.mk

all: $(EARLY_OBJS) $(OUTPUT)
idt_handlers.o: idt_handlers.S ../ia_common/idt_common.h
	@$(CC) -I../ia_common -I. -DASM_FILE -c $< -o $@

header.o: header.S ../../../common/include/multiboot.h
	@$(CC) -I. -I../../../common/include -c $< -o $@

early_pages.o: early_pages.c
	@$(CC) $(CFLAGS) -c $< -o $@

