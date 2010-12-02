OBJS = bochs_cons.o

bochs.a : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

bochs.o :

.PHONY : clean
clean :
	-rm bochs.a $(OBJS)

depend: bochs_cons.c
	$(MAKEDEPEND) $(INCLUDES) $^
