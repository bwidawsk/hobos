# This file is to be included by any standard subdirectory within the kernel
# source tree. It will generate dependencies using makedepend. This is
# accomplished by having the Makefile in the directory define special variables
# (see below for the special variables since comments always get stale).
OBJS?=$(subst .c,.o,$(abspath $(SOURCES)))
# This is based on what Automake does for verbosity.
#V=0
v_CC_0=@echo "  CC     $(subst $(ROOTDIR)/, , $<)"; $(CC)
v_CC_1=$(CC)
v_CC_=$(v_CC_0)
_CC=$(v_CC_$(V))

v_LD_0=@$(LD)
v_LD_1=@echo "  LD     $(subst $(ROOTDIR)/, , $<)"; $(LD)
v_LD_2=$(LD)
v_LD_=$(v_LD_0)
_LD=$(v_LD_$(V))

%.o: %.c
	$(_CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): subdirs $(EARLY_OBJS) $(OBJS) depend
	$(_LD) $(LDFLAGS) -r $(EARLY_OBJS) $(OBJS) -o $@

clean:
	-@rm -f $(OUTPUT) $(EARLY_OBJS) $(OBJS) depend depend.bak
	-@rm -f $(EXTRA_FILES)
	 -@for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 clean || exit 1) || exit 1; done

.PHONY: subdirs
subdirs:
	-@for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done

depend: $(SOURCES)
	-@rm -f depend
	@touch depend # Is this even necessary?
	-@$(MAKEDEPEND) -DKERNEL -fdepend -Y $(INCLUDES) -- $(ESOTERIC_INCLUDES) -- $^

-include depend

.PHONY: clean subdirs
