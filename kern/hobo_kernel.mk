CFLAGS+=$(INCLUDES)

OBJS=$(subst .c,.o,$(SOURCES))

$(OUTPUT) : $(OBJS) depend
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT) $(OBJS)
	-rm depend
	-rm depend.bak

depend: $(SOURCES)
	rm -f depend
	touch depend
	$(MAKEDEPEND) -fdepend -Y $(INCLUDES) $^

-include depend
