#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifndef KERNEL
#error POO ON YOU
#endif

#define CONSOLE_KEY sect_cons

#define CONSOLE_CREATE_LIST CTLIST_CREATE(CONSOLE_KEY, struct console_driver *); 

#define CONSOLE_DECLARE(cons_name, cinit, cputc, cgetc, ccheckc, val) \
	static struct console_driver cons_name = { \
		.name = #cons_name, \
		.init = cinit, \
		.putc = cputc, \
		.getc = cgetc, \
		.checkc = ccheckc, \
		.awesomeness = val \
		}; \
	CTLIST_ELEM_ADD(CONSOLE_KEY, cons_name##_list_ptr, struct console_driver *, (struct console_driver *)&cons_name);

/* Gives a pointer to each console device. */
#define CONSOLE_FOREACH(elem, garbage) \
	CTLIST_FOREACH(elem, CONSOLE_KEY, garbage)

struct console_driver {
	char *name;
	void (*init)(struct console_driver *);
	void (*putc)(struct console_driver *me_cons, unsigned char);
	char (*getc)(struct console_driver *me_cons);
	int (*checkc)(struct console_driver *me_cons);
	void *pvt;
	int awesomeness;
};

void console_init();
void console_putc(char c);
char console_getc();
void console_puts(char *s);

void start_interactive_console();
#endif