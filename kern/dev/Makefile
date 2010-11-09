DIRS = bochs uart pci block timer pic
OBJS = bochs/bochs.a uart/uart.a pci/pci.a block/block.a timer/timer.a pic/pic.a
 
OUTPUT = devices.a

.PHONY : $(OUTPUT)
$(OUTPUT) : 
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
