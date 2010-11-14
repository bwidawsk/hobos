SOURCES=main.c noarch.c console.c mutex.c bs_commands.c
OBJS=$(subst .c,.o,$(SOURCES))
OUTPUT=core.a

CFLAGS+=-I../../common/include

%.d: %.c
	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: vers.o
vers.o:

$(OUTPUT) : $(OBJS) vers.o
	$(LD) $(LDFLAGS) -r $(OBJS) -o $@

.PHONY : clean
clean :
	-rm *.d
	-rm vers.o
	-rm $(OUTPUT) $(OBJS)

ifneq ($(MAKECMDGOALS),clean)
	include $(subst .c,.d,$(SOURCES))
endif
