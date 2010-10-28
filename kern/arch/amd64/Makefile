# I got information on how to do the dependency stuff from here:
# http://www.makelinux.net/make3/make3-CHP-2-SECT-7.html
# (copied in my other makefiles)

# Special objects which don't follow normal rules
HEADEROBJS=header.o early_pages.o
HEADEROBJ_CFLAGS=-ansi -std=c99 -ffreestanding -nostartfiles -nodefaultlibs -nostdlib -DKERNEL -I. -I../../ -I../ia_common -I../../../common/include -I../../../common/include/amd64 -DAMD64 -mcmodel=kernel

CFLAGS+=-I. -I../../ -I../ia_common -I../../../common/include -I../../../common/include/amd64
SOURCES=begin.c mm_amd64.c atomic_amd64.c idt.c reset.c
OBJS=$(subst .c,.o,$(SOURCES)) idt_handlers.o

all : kernel 
	
%.d: %.c
	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

idt_handlers.o : idt_handlers.S ../ia_common/idt_common.h
	$(CC) $(CFLAGS) -DASM_FILE -c $< -o $@

header.o : header.S \
		../../../common/include/multiboot.h
	$(CC) $(HEADEROBJ_CFLAGS) -DASM_FILE -c $< -o $@

early_pages.o : early_pages.c
	$(CC) $(HEADEROBJ_CFLAGS) -c $< -o $@

kernel : $(OBJS) $(HEADEROBJS)
	$(LD) $(LDFLAGS)  $(OBJS) $(HEADEROBJS) -r -o arch.a

.PHONY: clean
clean :
	-rm arch.a
	-rm *.o
	-rm *.d
	
include $(subst .c,.d,$(SOURCES))
