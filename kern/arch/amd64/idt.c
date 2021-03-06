#include <console.h> // start_interactive_console
#include "gdt.h" // need KERNEL_CS64 in SET_IDT_INTR macro
#include "include/irq.h" //AKA <arch/irq.h>
#include "idt_common.h"
#include "include/asm.h"
#include "include/arch.h"

static struct idt_entry idt[MAX_IDT_ENTRIES];

/* TODO:
 * idt_handlers.S, I push a 0 error code in certain cases, I need to pop as well
 */
void
dflt_c_handler(struct trap_frame64 *tf) {
	static struct arch_state fault_state;

	printf("got exception %s (#%d)\n", get_fault_type(tf->tf_which),
		   tf->tf_which);
	printf("tf_rip = %p\n", tf->tf_rip);
	printf("tf_cs = %p\n", tf->tf_cs);

	if (tf->tf_which == T_PAGE_FAULT)
		printf("fault address == 0x%x\n", read_cr2());

	struct console_info ci;
	arch_from_trapframe(&fault_state, tf);
	ci.arch_state = &fault_state;
	start_interactive_console(&ci);
	while(1);
}

extern void (*generic_handler)();

#include "extern_vector_table.h"
void
setup_exception_handlers() {
	int i;
	for( i = 0; i < MAX_IDT_ENTRIES; i++) {
		// set up a known handler for every entry to help debugging
		SET_IDT_INTR(i, &generic_handler);
		//if (i > 31)
		//	SET_IDT_INTR(i, external_idt_vectors[i - 32]);
	}

	// Set the default vectors which we actually care about
	SET_DEFAULT_VECTORS;

	struct idt_descriptor idtr = {
		.limit = (sizeof(struct idt_entry) * MAX_IDT_ENTRIES) - 1,
		//.base = KVA_TO_PA((uint64_t)&idt)
		.base = (uint64_t)&idt
	};
	lidt((void *)&idtr);
}

void
arch_setup_irq(int vector) {
	SET_IDT_INTR(IRQ_EXTERNAL + vector, external_idt_vectors[vector]);
}

void
arch_release_irq(int vector) {
	SET_IDT_INTR(IRQ_EXTERNAL + vector, generic_handler);
}
