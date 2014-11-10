OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
EXES := $(OBJECTS:%.o=%)

all: $(EXES)

#special stuff goes here
ext2_tool: CFLAGS += -D_GNU_SOURCE -I$(KERNEL_PATH)/fs/ext2/ -lm

md5: md5.o $(COMMON)/src/md5_base.o
	$(CC) $(CLAGS) $(COMMON)/src/md5_base.o -o $@ $<
elf_parse: CFLAGS += -I$(COMMON)

.PHONY: clean
clean :
	-@rm -f $(OBJECTS) $(EXES)
