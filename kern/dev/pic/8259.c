#include <stdint.h>
#include <arch/asm.h>
#include <atomic_generic.h>

#define MASTER_CMD_PORT 0x20
#define MASTER_DATA_PORT 0x21
#define SLAVE_CMD_PORT 0xa0
#define SLAVE_DATA_PORT 0xa1

#define PIC8259_EOI		0x20

// ICW1
#define PIC8259_ICW1_ICW4_NEEDED 0x1
#define PIC8259_ICW1_SINGLE 0x2
#define PIC8259_ICW1_ADI4 0x4
#define PIC8259_ICW1_LTIM 0x8 /* Level triggered interrupt mode */
#define PIC8259_ICW1_INIT	0x10 /* A0 = 0, D4 = 1 */
#define PIC8259_ICW1_DEFAULT \
	(PIC8259_ICW1_ICW4_NEEDED | PIC8259_ICW1_INIT)

#define PIC8259_ICW4_8086 0x1
#define PIC8259_ICW4_AEOI 0x2
#define PIC8259_ICW4_BUFFERED_SLAVE 0x8
#define PIC8259_ICW4_BUFFERED_MASTER 0xC
#define PIC8259_ICW4_SPECIAL_FULLY_NESTED 0x10

#define PIC8259_OCW2_NOP (0x2 << 5)

#define PIC8259_OCW3_READ_IRR (0x8 | 0x2)
#define PIC8259_OCW3_READ_ISR (0x8 | 0x3)

static uint64_t master_inited = 0;
static uint64_t slave_inited = 0;

enum which_8259 {
	PIC8259_MASTER = 1, 
	PIC8259_SLAVE = 2,
	PIC8259_BOTH = 3
};

enum which_ir {
	PIC8259_IRR = 1, 
	PIC8259_ISR = 2,
};

void 
pic8259_eoi(uint8_t irq) {
	if(irq >= 8) {
		KASSERT(slave_inited == 1, ("%s: slave not inited\n", __FUNCTION__));
		outb(PIC8259_EOI, SLAVE_CMD_PORT);
	}
	KASSERT(master_inited == 1, ("%s: master not inited\n", __FUNCTION__));
	outb(PIC8259_EOI, MASTER_CMD_PORT);
}

void
pic8259_init(enum which_8259 which ) {
	if (which & PIC8259_MASTER) {
		// Start the initialization sequence
		outb(PIC8259_ICW1_DEFAULT, MASTER_CMD_PORT);
	
		// Set the vector to start at 32
		outb(0x20, MASTER_DATA_PORT);
	
		// This says which bit has the slave
		// historically, it's 4
		outb(0x4, MASTER_DATA_PORT);

		//8086 mode with no aeoi	
		outb(PIC8259_ICW4_8086, MASTER_DATA_PORT);

		master_inited = 1;
	}	
	
	if (which & PIC8259_SLAVE) {
		// Start the initialization sequence
		outb(PIC8259_ICW1_DEFAULT, SLAVE_CMD_PORT);
	
		// Set the vector to start at 40
		outb(0x28, SLAVE_DATA_PORT);
	
		// This says which bit on the master we're
		// connected to
		outb(0x2, SLAVE_DATA_PORT);
	
		//8086 mode with no aeoi	
		outb(PIC8259_ICW4_8086, SLAVE_DATA_PORT);

		slave_inited = 1;
	}
}
uint8_t
pic8259_get_ir(enum which_8259 which, uint8_t which_ir) {
	KWARN(IS_POW2(which), ("BUG: pic8259_get_irr called for multiple pics\n"));
	uint8_t which_data_port;	
	uint8_t which_cmd_port;	
	uint8_t ret;

	if (which & PIC8259_SLAVE) {
		KASSERT(slave_inited == 1, ("%s: slave not inited\n", __FUNCTION__));
		which_data_port = SLAVE_DATA_PORT;
		which_cmd_port = SLAVE_CMD_PORT;
	} else {
		KASSERT(master_inited == 1, ("%s: master not inited\n", __FUNCTION__));
		which_data_port = MASTER_DATA_PORT;
		which_cmd_port = MASTER_CMD_PORT;
	}

	uint8_t current_mask = inb(which_data_port);
	outb(current_mask, which_data_port);
	outb(PIC8259_OCW2_NOP, which_cmd_port);
	outb(which_ir, which_cmd_port);
	ret = inb(which_cmd_port);

	return ret;
}

void
pic8259_print_irrs() {
	printf("Master IRR = %x\n", pic8259_get_ir(PIC8259_MASTER, PIC8259_OCW3_READ_IRR));
	printf("Slave IRR = %x\n", pic8259_get_ir(PIC8259_SLAVE, PIC8259_OCW3_READ_IRR));
}

void
pic8259_print_isrs() {
	printf("Master ISR = %x\n", pic8259_get_ir(PIC8259_MASTER, PIC8259_OCW3_READ_ISR));
	printf("Slave ISR = %x\n", pic8259_get_ir(PIC8259_SLAVE, PIC8259_OCW3_READ_ISR));
}
