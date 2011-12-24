#ifndef _GDT_AMD64_H_
#define _GDT_AMD64_H_

#define KERNEL_CS_IDX 1
#define KERNEL_CS 0x10
#define KERNEL_DS_IDX 2
#define KERNEL_DS 0x20
#define KERNEL_CS64_IDX 3
#define KERNEL_CS64 0x30
#define KERNEL_TSS_IDX 4
#define KERNEL_TSS 0x40

#ifndef ASM_FILE
/* padded to 16 bytes */
struct segment_descriptor {
    uint64_t limit15_0:16;
    uint64_t base23_0:24;
    uint64_t type:4;
	uint64_t S:1;
    uint64_t DPL:2;		/* descriptor priority level */
    uint64_t P:1;			/* present */
    uint64_t limit19_16:4;
    uint64_t AVL:1;		/* available for system use*/
    uint64_t L:1;			/* long mode */
    uint64_t D_B:1;		/* operation size */
    uint64_t G:1;			/* granularity*/
    uint64_t base31_24:40;
	uint64_t pad:32;
} __attribute__((packed));

struct tss_descriptor {
	uint64_t limit15_0:16;
	uint64_t base23_0:24;
	uint64_t type:4;
	uint64_t sbz:1;
	uint64_t DPL:2;		/* descriptor priority level */
	uint64_t P:1;		/* present */
	uint64_t limit19_16:4;
	uint64_t AVL:1;
	uint64_t sbz2:2;
	uint64_t G:1;		/* Granularity */
	uint64_t base63_24:40;
	uint64_t sbz3:32;
} __attribute__((packed));

struct gdt_descriptor {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct gdt_descriptor64 {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

enum {
	DATA_RO = 0,
	DATA_RW = 2,
	DATA_RO_EDOWN = 4,
	DATA_RW_EDOWN = 6,
	CODE_X = 8,
	CODE_RX = 10,
	CODE_X_CONFORMING = 12,
	CODE_RX_CONFORMING = 14
};
//TODO: this is repeated in idt_64, clean it up
enum {
	SYSTEM_UPPER8 = 0,
	SYSTEM_LDT = 2,
	SYSTEM_TSS64 = 9,
	SYSTEM_TSS64_BUSY = 11,
	SYSTEM_CALL64 = 12,
	SYSTEM_INT64 = 14,
	SYSTEM_TRAP64 = 15
};

#define ZERO_SEGMENT \
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }


#define KERNEL_CODE32_SEGMENT \
	{ \
		0xFFFF, /* limit */ \
		0, /* base */ \
		CODE_RX, /* type */ \
		1, /* code or data */ \
		0, /* ring 0 */ \
		1, /* present */ \
		0xF, /* limit */ \
		0, /* avl */ \
		0, /* not long mode */ \
		1, /* 32 bit */ \
		1, /* 4k granularity */ \
		0  /* base */ \
	}


#define KERNEL_DATA32_SEGMENT \
	{ \
		0xFFFF, /* limit */ \
		0, /* base */ \
		DATA_RW, /* type */ \
		1, /* code or data */ \
		0, /* ring 0 */ \
		1, /* present */ \
		0xF, /* limit */ \
		0, /* avl */ \
		0, /* should be 0 */ \
		1, /* 32 bit */ \
		1, /* 4k granularity */ \
		0  /* base */ \
	}

#define KERNEL_CODE64_SEGMENT \
	{ \
		0, /* limit */ \
		0, /* base */ \
		CODE_RX, /* type */ \
		1, /* code or data */ \
		0, /* ring 0 */ \
		1, /* present */ \
		0, /* limit */ \
		0, /* avl */ \
		1, /* long mode */ \
		0, /* 32 bit */ \
		1, /* 4k granularity */ \
		0  /* base */ \
	}

#define TSS_SEGMENT \
	{ \
		0, /* limit */ \
		0, /* base */ \
		SYSTEM_TSS64, /* type */ \
		0, /* system */ \
		0, /* ring 0 */ \
		1, /* present */ \
		0, /* limit */ \
		0, /* avl */ \
		0, /* long mode */ \
		0, /* 32 bit */ \
		1, /* 4k granularity */ \
		0  /* base */ \
	}
#define GDT_SEGMENTS \
	ZERO_SEGMENT, KERNEL_CODE32_SEGMENT, \
	KERNEL_DATA32_SEGMENT, KERNEL_CODE64_SEGMENT, \
	TSS_SEGMENT
#define NUM_SEGMENTS 5

#ifndef KERNEL_CS
	#error must have a KERNEL SEGMENT defined
#endif

#ifndef KERNEL_DS
	#error must have a KERNEL SEGMENT defined
#endif

/* TODO: lgdt has become AMD64 specific 
 * NB: bochs and qemu seem to behave differently regarding the iretq. qemu is
 * only popping 4 qwords, while bochs pops 5. ie. qemu needs the extra pop
 */
static
inline void lgdt(const void *gdtdesc) {
__asm__ volatile (
		"lgdt (%0)\n\t"
		"movq %%ss, %%rax\n\t"
		"pushq %%rax\n\t"
		"pushq %%rsp\n\t"
		"pushfq\n\t"
		"pushq %1\n\t"
		"pushq $place_to_jump\n\t"
		"iretq\n\t"
		"place_to_jump:\n\t"
		"movq %2, %%rax\n\t"
		"movq %%rax, %%ds\n\t"
		"movq %%rax, %%es\n\t"
		"movq %%rax, %%fs\n\t"
		"movq %%rax, %%gs\n\t"
		"movq %%rax, %%ss\n\t"
#ifndef BOCHS
		"popq %%rbx\n\t"
#endif
		:
		: "p" (gdtdesc), "i" (KERNEL_CS64), "i" (KERNEL_DS)
		: "%rax", "%rbx"
		);
}
#endif // ASM_FILE

#endif

