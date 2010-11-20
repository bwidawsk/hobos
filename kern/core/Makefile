SOURCES=main.c noarch.c console.c mutex.c bs_commands.c interrupt.c timer.c
SOURCES+=vers.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=core.a

CFLAGS+=-I../../common/include

%.d: %.c
	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUTPUT) : $(OBJS)
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm *.d
	-rm $(OUTPUT) $(OBJS)

ifneq ($(MAKECMDGOALS),clean)
	include $(subst .c,.d,$(SOURCES))
endif
