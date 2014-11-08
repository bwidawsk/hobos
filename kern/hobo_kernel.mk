# This file is to be included by any standard subdirectory within the kernel
# source tree. It will generate dependencies using makedepend. This is
# accomplished by having the Makefile in the directory define special variables
# (see below for the special variables since comments always get stale).
OBJS=$(subst .c,.o,$(abspath $(SOURCES)))

$(OUTPUT): $(OBJS) depend
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY: clean
clean:
	-@rm -f $(OUTPUT) $(OBJS) depend depend.bak

depend: $(SOURCES)
	-@rm -f depend
	@touch depend # Is this even necessary?
	-@$(MAKEDEPEND) -DKERNEL -fdepend -Y $(INCLUDES) -- $(ESOTERIC_INCLUDES) -- $^

-include depend
