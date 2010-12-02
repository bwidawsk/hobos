SOURCES= ata_block.c ata_io.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=block.a

CFLAGS+=-I../../common/include -I../../

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS)

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
