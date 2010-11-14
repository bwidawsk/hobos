#include "stdint.h"
#include "gdt.h" // need KERNEL_CS64 in SET_IDT_INTR macro
#include "idt_amd64.h"
#include "idt_common.h"
#include "include/asm.h"

struct idt_entry idt[MAX_IDT_ENTRIES];

uint8_t temp_exception_stack[4096] __attribute__ ((aligned (4096)));

void 
undefined_c_handler() {
	pic8259_print_irrs();
	pic8259_print_isrs();
}

/* TODO:
 * idt_handlers.S, I push a 0 error code in certain cases, I need to pop as well
 */
void
dflt_c_handler(struct trap_frame64 *tf) {
	printf("got exception #%d\n", tf->tf_which);
	printf("tf_rip = %p\n", tf->tf_rip);
	printf("tf_cs = %p\n", tf->tf_cs);

	if (tf->tf_which == T_PAGE_FAULT) {
		printf("fault address == 0x%x\n", read_cr2());
	} else if (tf->tf_which == T_UNDEF_FAULT) {
		start_interactive_console();
	}
	printf("\n");
	
	while(1);
}

extern void (*undefined_handler)();

void
setup_exception_handlers() {
	int i;
	for( i = 0; i < MAX_IDT_ENTRIES; i++) {
		// set up a known handler for every entry to help debugging
		SET_IDT_INTR(i, &undefined_handler);
	}

	// Set the default vectors which we actually care about
	SET_DEFAULT_VECTORS

	struct idt_descriptor idtr = {
		.limit = (sizeof(struct idt_entry) * MAX_IDT_ENTRIES) - 1,
		//.base = KVA_TO_PA((uint64_t)&idt)
		.base = (uint64_t)&idt
	};
	lidt((void *)&idtr);
}