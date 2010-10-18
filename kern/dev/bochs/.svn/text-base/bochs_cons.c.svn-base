#include <stdint.h>
#include "console.h"

#include "../../arch/amd64/inlined_asm_amd64.h"

#define BOCHS_CONS_AWESOMENESS 1
#define BOCHS_DEBUG_PORT 0xe9

static void bochs_cons_init(struct console_driver *me_cons);
static void bochs_cons_putc(struct console_driver *me_cons, unsigned char c);
static char bochs_cons_getc(struct console_driver *me_cons);
static int bochs_cons_checkc(struct console_driver *me_cons);
CONSOLE_DECLARE(bochs_cons, bochs_cons_init, bochs_cons_putc, bochs_cons_getc, bochs_cons_checkc, BOCHS_CONS_AWESOMENESS);

static void bochs_cons_init(struct console_driver *me_cons) {

}

static void bochs_cons_putc(struct console_driver *me_cons, unsigned char c) {
	outb(c, BOCHS_DEBUG_PORT);
}

static char bochs_cons_getc(struct console_driver *me_cons) { 
	return -1;
}

static int bochs_cons_checkc(struct console_driver *me_cons) {
	return 0;
}

void
early_putc(char c) {
	outb(c, BOCHS_DEBUG_PORT);
}
