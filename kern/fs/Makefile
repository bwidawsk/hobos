DIRS = vfs ext2
OBJS = vfs/vfs.a ext2/ext2.a

OUTPUT = fs.a

.PHONY : $(OUTPUT)
$(OUTPUT):
	@for dir in $(DIRS); do (cd $$dir; ${MAKE} $1 || exit 1) || exit 1; done
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

# TODO: Move this logic to hobo_kernel.mk
.PHONY : clean
clean :
	-@rm -f $(OUTPUT)
	@for dir in $(DIRS); do (cd $$dir; ${MAKE} clean $1 || exit 1) || exit 1; done
