SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o .h
CC=clang
#CC=gcc
CFLAGS=-idirafter../common/include -g3
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
EXES := $(OBJECTS:%.o=%)

all: md5 ext2_tool $(EXES)

#special stuff goes here
ext2_tool: CFLAGS += -D_GNU_SOURCE -I../kern/fs/ext2/ -lm

md5: CFLAGS += -I ../common/src/
md5: md5.o ../common/src/md5_base.o
	$(CC) $(CLAGS) ../common/src/md5_base.o -o $@ $<

.PHONY: clean
clean :
	-rm $(OBJECTS) $(EXES)
