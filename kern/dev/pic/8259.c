#include <stdint.h>
#include <arch/asm.h>

#define MASTER_CMD_PORT 0x20
#define MASTER_DATA_PORT 0x21
#define SLAVE_CMD_PORT 0xa0
#define SLAVE_DATA_PORT 0xa1

#define PIC_EOI		0x20a

// ICW1
#define PIC_ICW1_ICW4_NEEDED 0x1
#define PIC_ICW1_SINGLE 0x2
#define PIC_ICW1_ADI4 0x4
#define PIC_ICW1_LTIM 0x8 /* Level triggered interrupt mode */
#define PIC_ICW1_INIT	0x10 /* A0 = 0, D4 = 1 */
#define PIC_ICW1_DEFAULT \
	(PIC_ICW1_ICW4_NEEDED | PIC_ICW1_INIT)

#define PIC_ICW4_8086 0x1
#define PIC_ICW4_AEOI 0x2
#define PIC_ICW4_BUFFERED_SLAVE 0x8
#define PIC_ICW4_BUFFERED_MASTER 0xC
#define PIC_ICW4_SPECIAL_FULLY_NESTED 0x10

#define PIC_OCW2_NOP (0x2 << 5)

#define PIC_OCW3_READ_IRR (0x8 | 0x2)
#define PIC_OCW3_READ_ISR (0x8 | 0x3)

void 
pic_eoi(uint8_t irq) {
	if(irq >= 8)
		outb(PIC_EOI, SLAVE_CMD_PORT);
	outb(PIC_EOI, MASTER_CMD_PORT);
}

void
pic_init() {
	// Start the initialization sequence
	outb(PIC_ICW1_DEFAULT, MASTER_CMD_PORT);

	// Set the vector to start at 32
	outb(0x20, MASTER_DATA_PORT);

	// This says which bit has the slave
	// historically, it's 4
	outb(0x4, MASTER_DATA_PORT);

	//8086 mode with no aeoi	
	outb(PIC_ICW4_8086, MASTER_DATA_PORT);

	// Start the initialization sequence
	outb(PIC_ICW1_DEFAULT, SLAVE_CMD_PORT);

	// Set the vector to start at 40
	outb(0x28, SLAVE_DATA_PORT);

	// This says which bit on the master we're
	// connected to
	outb(0x2, SLAVE_DATA_PORT);

	//8086 mode with no aeoi	
	outb(PIC_ICW4_8086, SLAVE_DATA_PORT);
}

void
pic_print_irrs() {
	uint8_t current_mask_master = inb(MASTER_DATA_PORT);
	uint8_t current_mask_slave = inb(SLAVE_DATA_PORT);

	// leave the mask the same
	outb(current_mask_master, MASTER_DATA_PORT);

	// do nothing to OCW2
	outb(PIC_OCW2_NOP, MASTER_CMD_PORT);

	// set next read as IRR
	outb(PIC_OCW3_READ_IRR, MASTER_CMD_PORT);
	uint8_t master_irr = inb(MASTER_CMD_PORT);
	printf("Master IRR = %x\n", master_irr);
}

void
pic_print_isrs() {
	uint8_t current_mask_master = inb(MASTER_DATA_PORT);
	uint8_t current_mask_slave = inb(SLAVE_DATA_PORT);

	// leave the mask the same
	outb(current_mask_master, MASTER_DATA_PORT);

	// do nothing to OCW2
	outb(PIC_OCW2_NOP, MASTER_CMD_PORT);

	// set next read as ISR
	outb(PIC_OCW3_READ_ISR, MASTER_CMD_PORT);
	uint8_t master_isr = inb(MASTER_CMD_PORT);
	printf("Master ISR = %x\n", master_isr);
}
