#include <interrupt.h> // request_irq
#include <arch/asm.h>
#include <arch/atomic.h>
#include "pic.h"

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
#define PIC8259_OCW3_NOP (0x8)

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

static void
pic8259_eoi(int irq) {
	if(irq >= 8) {
		KASSERT(slave_inited, ("%s: slave not inited\n"), __FUNCTION__);
		outb(PIC8259_EOI, SLAVE_CMD_PORT);
	}
	KASSERT(master_inited, ("%s: master not inited\n"), __FUNCTION__);
	outb(PIC8259_EOI, MASTER_CMD_PORT);
}

static void
pic8259_mask(int irq) {
	uint8_t which_data_port;	
	uint8_t which_cmd_port;	

	if(irq >= 8) { 
		which_data_port = SLAVE_DATA_PORT;
		which_cmd_port = SLAVE_CMD_PORT;
	} else {
		which_data_port = MASTER_DATA_PORT;
		which_cmd_port = MASTER_CMD_PORT;
	}
	volatile uint8_t current_mask = inb(which_data_port);
	current_mask |= (1 << (irq %8));
	outb(current_mask, which_data_port);
	outb(PIC8259_OCW2_NOP, which_cmd_port);
	outb(PIC8259_OCW3_NOP, which_cmd_port);
}

static void
pic8259_unmask(int irq) {
	uint8_t which_data_port;	
	uint8_t which_cmd_port;	

	if(irq >= 8) { 
		which_data_port = SLAVE_DATA_PORT;
		which_cmd_port = SLAVE_CMD_PORT;
	} else {
		which_data_port = MASTER_DATA_PORT;
		which_cmd_port = MASTER_CMD_PORT;
	}
	uint8_t current_mask = inb(which_data_port);
	current_mask &= ~(1 << (irq %8));
	outb(current_mask, which_data_port);
	outb(PIC8259_OCW2_NOP, which_cmd_port);
	outb(PIC8259_OCW3_NOP, which_cmd_port);
}
static int
master_spurious(void *data) {
	printf("master spurious\n");
	return 0;
}

static int
slave_spurious(void *data) {
	printf("slave spurious\n");
	return 0;
}

static int
pic8259_init(int which) {
	int i;
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
		for(i = 0; i < 8; i++)
			pic8259_mask(i);
		master_inited = 1;
		register_irq(7, master_spurious, NULL);
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

		for(i = 8; i < 16; i++)
			pic8259_mask(i);
		slave_inited = 1;
		register_irq(15, slave_spurious, NULL);
	}
	return 0;
}

uint8_t
pic8259_get_ir(enum which_8259 which, uint8_t which_ir) {
	KWARN(IS_POW2(which), ("BUG: pic8259_get_irr called for multiple pics\n"));
	uint8_t which_data_port;	
	uint8_t which_cmd_port;	
	uint8_t ret;

	if (which & PIC8259_SLAVE) {
		KASSERT(slave_inited == 1, ("%s: slave not inited\n"), __FUNCTION__);
		which_data_port = SLAVE_DATA_PORT;
		which_cmd_port = SLAVE_CMD_PORT;
	} else {
		KASSERT(master_inited == 1, ("%s: master not inited\n"), __FUNCTION__);
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

struct pic_dev pic_8259 = {
	.init = pic8259_init,
	.mask = pic8259_mask,
	.unmask = pic8259_unmask,
	.eoi = pic8259_eoi
};


#include <bs_commands.h>
static void *
pic8259_dump(struct console_info *info, int argc, char *argv[]) {
	pic8259_print_irrs();
	pic8259_print_isrs();
	return NULL;
}
void
pic8259_help() {
	printf("Dump 8259 state\n");
}
BS_COMMAND_DECLARE(dump_8259, pic8259_dump, pic8259_help);
