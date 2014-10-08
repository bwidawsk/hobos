include Makefile.base
all:

debug: CFLAGS+=-ggdb3 -DHALT_ON_ENTRY
debug: override LD_SCRIPT=debug.ld
debug: all
