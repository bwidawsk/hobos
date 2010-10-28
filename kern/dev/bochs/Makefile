OBJS = bochs_cons.o

bochs.a : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

bochs.o :

.PHONY : clean
clean :
	-rm *.d
	-rm bochs.a $(OBJS)
