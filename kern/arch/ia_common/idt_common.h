#ifndef _IDT_COMMON_H_
#define _IDT_COMMON_H_

#define MAX_IDT_ENTRIES 256

#define	T_DIVIDE_FAULT	0
#define	T_DEBUG			1
#define	T_NMI_INT		2
#define	T_BKPT_TRAP		3
#define	T_OVRFLW_TRAP	4
#define	T_BOUND_FAULT	5
#define	T_UNDEF_FAULT	6
#define	T_NOMATH_FAULT	7
#define	T_DOUBLE_FAULT	8
#define	T_COPROC_FAULT	9
#define	T_TSS_FAULT		10
#define	T_SEG_FAULT		11
#define	T_STACK_FAULT	12
#define	T_GP_FAULT		13
#define	T_PAGE_FAULT	14
#define	T_RSVD			15
#define	T_MATH_FAULT	16
#define	T_ALIGN_FAULT	17
#define	T_MACH_ABORT	18
#define	T_SIMD_FAULT	19

#define IRQ_EXTERNAL 32
#define IRQ_MAX (MAX_IDT_ENTRIES - 1)

#ifdef ASM_FILE
 #define IDTVEC(name) \
	.align 64; \
	.globl X ## name; \
	.type X ## name ,@function; \
	X ## name:
#else
 #define IDTVEC(name)   X ## name

/* default handlers for all traps, aborts, and faults */
extern void IDTVEC(T_DIVIDE_FAULT)(void);
extern void IDTVEC(T_DEBUG)(void);
extern void IDTVEC(T_NMI_INT)(void);
extern void IDTVEC(T_BKPT_TRAP)(void);
extern void IDTVEC(T_OVRFLW_TRAP)(void);
extern void IDTVEC(T_BOUND_FAULT)(void);
extern void IDTVEC(T_UNDEF_FAULT)(void);
extern void IDTVEC(T_NOMATH_FAULT)(void);
extern void IDTVEC(T_DOUBLE_FAULT)(void);
extern void IDTVEC(T_COPROC_FAULT)(void);
extern void IDTVEC(T_TSS_FAULT)(void);
extern void IDTVEC(T_SEG_FAULT)(void);
extern void IDTVEC(T_STACK_FAULT)(void);
extern void IDTVEC(T_GP_FAULT)(void);
extern void IDTVEC(T_PAGE_FAULT)(void);
extern void IDTVEC(T_RSVD)(void);
extern void IDTVEC(T_MATH_FAULT)(void);
extern void IDTVEC(T_ALIGN_FAULT)(void);
extern void IDTVEC(T_MACH_ABORT)(void);
extern void IDTVEC(T_SIMD_FAULT)(void);

#define SET_DEFAULT_VECTORS \
	SET_IDT_INTR(T_DIVIDE_FAULT, &IDTVEC(T_DIVIDE_FAULT)); \
	SET_IDT_INTR(T_DEBUG, &IDTVEC(T_DEBUG)); \
	SET_IDT_INTR(T_NMI_INT, &IDTVEC(T_NMI_INT)); \
	SET_IDT_INTR(T_BKPT_TRAP, &IDTVEC(T_BKPT_TRAP)); \
	SET_IDT_INTR(T_OVRFLW_TRAP, &IDTVEC(T_OVRFLW_TRAP)); \
	SET_IDT_INTR(T_BOUND_FAULT, &IDTVEC(T_BOUND_FAULT)); \
	SET_IDT_INTR(T_UNDEF_FAULT, &IDTVEC(T_UNDEF_FAULT)); \
	SET_IDT_INTR(T_NOMATH_FAULT, &IDTVEC(T_NOMATH_FAULT)); \
	SET_IDT_INTR(T_DOUBLE_FAULT, &IDTVEC(T_DOUBLE_FAULT)); \
	SET_IDT_INTR(T_COPROC_FAULT, &IDTVEC(T_COPROC_FAULT)); \
	SET_IDT_INTR(T_TSS_FAULT, &IDTVEC(T_TSS_FAULT)); \
	SET_IDT_INTR(T_SEG_FAULT, &IDTVEC(T_SEG_FAULT)); \
	SET_IDT_INTR(T_STACK_FAULT, &IDTVEC(T_STACK_FAULT)); \
	SET_IDT_INTR(T_GP_FAULT, &IDTVEC(T_GP_FAULT)); \
	SET_IDT_INTR(T_PAGE_FAULT, &IDTVEC(T_PAGE_FAULT)); \
	SET_IDT_INTR(T_RSVD, &IDTVEC(T_RSVD)); \
	SET_IDT_INTR(T_MATH_FAULT, &IDTVEC(T_MATH_FAULT)); \
	SET_IDT_INTR(T_ALIGN_FAULT, &IDTVEC(T_ALIGN_FAULT)); \
	SET_IDT_INTR(T_MACH_ABORT, &IDTVEC(T_MACH_ABORT)); \
	SET_IDT_INTR(T_SIMD_FAULT, &IDTVEC(T_SIMD_FAULT))

static inline const char *
get_fault_type(int f)
{
	static const char *fault[] = {
		"DIVIDE_FAULT",
		"DEBUG",
		"NMI_INT",
		"BKPT_TRAP",
		"OVRFLW_TRAP",
		"BOUND_FAULT",
		"UNDEF_FAULT",
		"NOMATH_FAULT",
		"DOUBLE_FAULT",
		"COPROC_FAULT",
		"TSS_FAULT",
		"SEG_FAULT",
		"STACK_FAULT",
		"GP_FAULT",
		"PAGE_FAULT",
		"RSVD",
		"MATH_FAULT",
		"ALIGN_FAULT",
		"MACH_ABORT",
		"SIMD_FAULT"
	};

	return fault[f];
}
extern void setup_exception_handlers();

static inline void
lidt(void *idtr) {
	__asm__ volatile ("lidt (%0)" : : "p"(idtr));
}
#endif
#endif
