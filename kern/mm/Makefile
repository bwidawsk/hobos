SOURCES=mm.c ds_allocator.c malloc.c
OBJS=$(subst .c,.o,$(SOURCES))
DEPS=$(subst .c,.d,$(SOURCES))
OUTPUT=mm.a

all : $(OUTPUT)

%.d: %.c
	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm *.d
	-rm $(OUTPUT) $(OBJS) $(DEPS)

ifneq ($(MAKECMDGOALS),clean)
	include $(subst .c,.d,$(SOURCES))
endif
