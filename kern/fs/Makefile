DIRS = ext2
OBJS = ext2/ext2.a
 
OUTPUT = fs.a

.PHONY : $(OUTPUT)
$(OUTPUT) : 
	for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OUTPUT)
	for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
