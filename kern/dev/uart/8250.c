#include <stdint.h>
#include <console.h>
#include "8250_regs.h"
#include "8250.h"

#define UART_CONS_AWESOMENESS 5

static void cons_init_8250(struct console_driver *me_cons);
static void cons_putc_8250(struct console_driver *me_cons, unsigned char c);
static char cons_getc_8250(struct console_driver *me_cons);
static int cons_checkc_8250(struct console_driver *me_cons);
CONSOLE_DECLARE(cons_8250, cons_init_8250, cons_putc_8250, cons_getc_8250, cons_checkc_8250, UART_CONS_AWESOMENESS);

int use_8250_io = 1;

/* Temporarily this stuff defaults to io */
#include <arch/asm.h>
static void
write_8250_reg(uint8_t reg, uint8_t val, void *ctrl) {
	outb(val, 0x3f8+reg);
}

static uint8_t
read_8250_reg(uint8_t reg, void *ctrl) {
	return inb(0x3f8+reg);
}

static void
write_divisor(uint32_t divisor, void *ctrl) {
	uint8_t orig_lcr = read_8250_reg(LCR_REG_8250, ctrl);
	write_8250_reg(LCR_REG_8250, orig_lcr | LCR_DLAB_ON, ctrl);
	write_8250_reg(DLL_REG_8250, DIVISOR_TO_DLL(divisor), ctrl);
	write_8250_reg(DLM_REG_8250, DIVISOR_TO_DLM(divisor), ctrl);
	write_8250_reg(LCR_REG_8250, orig_lcr, ctrl);
}

static void 
cons_init_8250(struct console_driver *me_cons) {
	me_cons->pvt = &use_8250_io;
	
	/* Disable interrupts */
	write_8250_reg(IER_REG_8250, 0, me_cons->pvt);
	
	/* Disable FIFOs */
	write_8250_reg(FCR_REG_8250, FCR_DISABLE, me_cons->pvt);
	
	/* Hardcoded for now 115200 8n1 */
	write_8250_reg(LCR_REG_8250, LCR_WORD_8bit | LCR_NO_PARITY | LCR_STOP_1bit |
		LCR_BREAK_DIS | LCR_DLAB_OFF, me_cons->pvt);
	write_divisor(115200, me_cons->pvt);
}

#define CONSOLE_NEEDS_STUPID_LINEFEED
static void 
cons_putc_8250(struct console_driver *me_cons, unsigned char c) {
	uint8_t thre;
	#ifdef CONSOLE_NEEDS_STUPID_LINEFEED
	if (c == '\n') {
		cons_putc_8250(me_cons, '\r');
	}
	#endif
	do {
		thre = read_8250_reg(LSR_REG_8250, me_cons->pvt);
	} while(!(thre & LSR_THRE_IDLE));
	
	write_8250_reg(THR_REG_8250, c, me_cons->pvt);
	
	// We don't know for sure that the character has been pushed out yet, but
	// I see no reason to wait.
	#if 0
	do {
		thre = read_8250_reg(LSR_REG_8250, me_cons->pvt);
	} while(!(thre & LSR_THRE_IDLE));
	#endif
}

static char 
cons_getc_8250(struct console_driver *me_cons) { 
	while(!cons_checkc_8250(me_cons));
	return read_8250_reg(RBR_REG_8250, me_cons->pvt);
}

static int
cons_checkc_8250(struct console_driver *me_cons) { 
	//uint8_t iir;
	//iir = read_8250_reg(IIR_REG_8250, me_cons->pvt);
	//return IIR_RXA(iir);
	uint8_t lsr;
	lsr = read_8250_reg(LSR_REG_8250, me_cons->pvt);
	return lsr & 1;
}