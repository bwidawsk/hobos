SUBDIRS=tools
CC=clang
# Common defines some header files with system names. So we must search after those.
COMMON=$(realpath ../common)
KERNEL_PATH=$(realpath $(PWD)/../kern/)
CFLAGS=-idirafter$(COMMON)/include -ggdb3
export CC
export CFLAGS
export COMMON
export KERNEL_PATH

.PHONY: $(SUBDIRS) fake

all: TARGET=all
clean: TARGET=clean
fake clean all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(TARGET)
