SOURCES=8250.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=uart.a

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS)

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
