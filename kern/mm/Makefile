SOURCES=mm.c ds_allocator.c malloc.c
OBJS=$(subst .c,.o,$(SOURCES))
DEPS=$(subst .c,.d,$(SOURCES))
OUTPUT=mm.a

all : $(OUTPUT)

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS) $(DEPS)

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
