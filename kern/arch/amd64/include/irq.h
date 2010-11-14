#ifndef _IDT_DEFINES_H_
#define _IDT_DEFINES_H_

#define TF_OLDSS_OFF 0x28
#define TF_OLDRSP_OFF 0x20
#define TF_OLDRFLAGS_OFF 0x18
#define TF_OLDCS_OFF 0x10
#define TF_OLDRIP_OFF 0x08
#define TF_ERR_OFF 0x00
#define TF_RAX_OFF -0x08
#define TF_RBX_OFF -0x10
#define TF_RCX_OFF -0x18
#define TF_RDX_OFF -0x20
#define TF_RDI_OFF -0x28
#define TF_RSI_OFF -0x30
#define TF_RBP_OFF -0x38
#define TF_RSP_OFF -0x40
#define TF_R8_OFF -0x48
#define TF_R9_OFF -0x50
#define TF_R10_OFF -0x58
#define TF_R11_OFF -0x60
#define TF_R12_OFF -0x68
#define TF_R13_OFF -0x70
#define TF_R14_OFF -0x78
#define TF_R15_OFF -0x80
#define TF_WHICH -0x88

#ifndef ASM_FILE

// TODO: pull this define from elsewhere
#define IDTVEC(name) X ## name

enum {
	IDT_UPPER8=0,
	IDT_LDT=2,
	IDT_TSS_AVL=9,
	IDT_TSS_BUSY=11,
	IDT_CALL_GATE=12,
	IDT_INTR_GATE=14,
	IDT_TRAP_GATE=15
};

struct trap_frame64 {
	uint64_t tf_which;
	uint64_t tf_r15;
	uint64_t tf_r14;
	uint64_t tf_r13;
	uint64_t tf_r12;
	uint64_t tf_r11;
	uint64_t tf_r10;
	uint64_t tf_r9;
	uint64_t tf_r8;
	uint64_t tf_rsp;
	uint64_t tf_rbp;
	uint64_t tf_rsi;
	uint64_t tf_rdi;
	uint64_t tf_rdx;
	uint64_t tf_rcx;
	uint64_t tf_rbx;
	uint64_t tf_rax;
	uint64_t tf_err;
	uint64_t tf_rip;
	uint64_t tf_cs;
	uint64_t tf_rflags;
	uint64_t tf_oldrsp;
	uint64_t tf_ss;
} __attribute__((packed));

struct interrupt_gate_descriptor {
    uint64_t offset15_0:16;
    uint64_t selector:16;
    uint64_t ist:3;
	uint64_t sbz:5;
	uint64_t type:4;
	uint64_t sbz2:1;
    uint64_t DPL:2;			/* descriptor priority level */
    uint64_t P:1;			/* present */
    uint64_t offset63_16:48;
	uint64_t rsvd:32;
} __attribute__((packed));

struct trap_gate_descriptor {
    uint64_t offset15_0:16;
    uint64_t selector:16;
    uint64_t ist:3;
	uint64_t sbz:5;
	uint64_t type:4;
	uint64_t sbz2:1;
    uint64_t DPL:2;			/* descriptor priority level */
    uint64_t P:1;			/* present */
    uint64_t offset63_16:48;
	uint64_t rsvd:32;
} __attribute__((packed));

struct idt_entry {
	union {
		struct interrupt_gate_descriptor intr_gate;
		struct trap_gate_descriptor trap_gate;
	}descriptor;
};

struct idt_descriptor{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

/* TODO: is it cool to leave the uint64_t? */
#define SET_IDT_INTR(vector, func)  do { \
		uint64_t temp = (uint64_t)func; \
		struct interrupt_gate_descriptor intr_gate = \
		{ \
			temp & 0xFFFFUL, \
			KERNEL_CS64, \
			1, /* ist */ \
			0, \
			IDT_INTR_GATE, \
			0, \
			0, \
			1, \
			temp >> 16, \
			0 \
		}; \
		idt[vector].descriptor.intr_gate = intr_gate; \
	}while(0);

void arch_setup_irq(int vector);
	
#endif
#endif