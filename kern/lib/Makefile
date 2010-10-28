SOURCES=early_putc.c libc_memfuncs.c libc_strfuncs.c freebsd_printf.c printf.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=lib.a

%.d: %.c
	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm *.d
	-rm $(OUTPUT) $(OBJS)

include $(subst .c,.d,$(SOURCES))
