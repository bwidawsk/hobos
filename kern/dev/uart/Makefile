SOURCES=8250.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=uart.a

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
ifneq ($(MAKECMDGOALS),clean)
	include $(subst .c,.d,$(SOURCES))
endif
