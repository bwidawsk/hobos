OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
EXES := $(OBJECTS:%.o=%)

all: $(EXES)

.PHONY: clean
clean :
	-@rm -f $(OBJECTS) $(EXES)
