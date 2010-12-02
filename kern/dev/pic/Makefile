SOURCES= 8259.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=pic.a

CFLAGS+=-I../../common/include -I../../

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS)

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
