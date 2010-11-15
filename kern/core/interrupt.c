#include <stdint.h>
#include <arch/irq.h>

/* Currently hardcoded to use 8259 */

// TODO: make this dynamic allocation
typedef int (*irq_handler)(void *);
irq_handler handlers[16] = {0};
void *handler_data[16] = {0};


void 
generic_c_handler(uint64_t vector) {
	KASSERT(vector != -1, ("No handler for interrupt\n"));
	printf("vector = %d\n", vector);
	pic8259_print_irrs();
	pic8259_print_isrs();
	handlers[vector](handler_data[vector]);
	pic8259_eoi(vector);
	return;
}

int
register_irq(int vector, int (*handler)(void *), void *data) {
	KASSERT(vector < 16, ("Can only handle 16 vectors for now\n"));
	KWARN(handlers[vector] == 0, ("Shared interrupts aren't support, overwriting vector %d\n", vector));

	handlers[vector] = handler;
	handler_data[vector] = data;
	arch_setup_irq(vector);
	// set the pic mask
	pic8259_unmask(vector);
	return 0;
}

void
unregister_irq(int vector) {
	pic8259_mask(vector);
	arch_release_irq(vector);
    handler_data[vector] = NULL;
	handlers[vector] = NULL;
}
