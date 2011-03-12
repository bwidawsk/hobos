CFLAGS+=$(INCLUDES)

OBJS=$(subst .c,.o,$(SOURCES))

$(OUTPUT) : $(OBJS) depend
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS) 

depend: $(SOURCES)
	$(MAKEDEPEND) $(INCLUDES) $^
