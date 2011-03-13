DIRS = generic bochs uart pci timer pic block legacy_pc
OBJS = generic/generic.a bochs/bochs.a uart/uart.a pci/pci.a timer/timer.a pic/pic.a block/block.a legacy_pc/pc.a
 
OUTPUT = devices.a

.PHONY : $(OUTPUT)
$(OUTPUT) : 
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
