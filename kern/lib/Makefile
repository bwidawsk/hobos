SOURCES=early_putc.c libc_memfuncs.c libc_strfuncs.c freebsd_printf.c printf.c quicksort.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=lib.a

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS) 

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
