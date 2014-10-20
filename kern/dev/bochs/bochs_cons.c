#include "console.h"

#include <arch/asm.h>

#define BOCHS_CONS_AWESOMENESS 1
#define BOCHS_DEBUG_PORT 0xe9

static void bochs_cons_putc(struct console_driver *me_cons, unsigned char c);
CONSOLE_DECLARE(bochs_cons, NULL, bochs_cons_putc, NULL, NULL, BOCHS_CONS_AWESOMENESS);

static void bochs_cons_putc(struct console_driver *me_cons, unsigned char c) {
	outb(c, BOCHS_DEBUG_PORT);
}

#if CONFIG_PORT_E9_PUTCHAR == 1
void
early_putc(char c) {
	outb(c, BOCHS_DEBUG_PORT);
}
#endif
