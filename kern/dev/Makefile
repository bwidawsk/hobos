DIRS = bochs uart pci
OBJS = bochs/bochs.a uart/uart.a pci/pci.a

OUTPUT = devices.a

.PHONY : $(OUTPUT)
$(OUTPUT) : 
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done