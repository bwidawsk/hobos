#include <stdint.h>
#include <arch/irq.h>

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
	KWARN(handlers[vector] == 0, ("Overwriting vector %d\n", vector));

	arch_setup_irq(vector);
	handlers[vector] = handler;
	handler_data[vector] = data;
	// set the pic mask
	pic8259_unmask(vector);
}
