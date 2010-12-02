SOURCES= pci.c pci_io_x86.c 
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=pci.a

CFLAGS+=-I../../common/include

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS)

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
