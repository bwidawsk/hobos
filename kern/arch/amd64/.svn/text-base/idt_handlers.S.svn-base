#include "idt_amd64.h"
#include "idt_common.h"

#define STORE_ALL_REGS \
	movq %rax, TF_RAX_OFF(%rsp) ;\
	movq %rbx, TF_RBX_OFF(%rsp) ;\
	movq %rcx, TF_RCX_OFF(%rsp) ;\
	movq %rdx, TF_RDX_OFF(%rsp) ;\
	movq %rsi, TF_RDI_OFF(%rsp) ;\
	movq %rdi, TF_RSI_OFF(%rsp) ;\
	movq %rbp, TF_RBP_OFF(%rsp) ;\
	movq %rsp, TF_RSP_OFF(%rsp) ;\
	movq %r8, TF_R8_OFF(%rsp) ;\
	movq %r9, TF_R9_OFF(%rsp) ;\
	movq %r10, TF_R10_OFF(%rsp) ;\
	movq %r11, TF_R11_OFF(%rsp) ;\
	movq %r12, TF_R12_OFF(%rsp) ;\
	movq %r13, TF_R13_OFF(%rsp) ;\
	movq %r14, TF_R14_OFF(%rsp) ;\
	movq %r15, TF_R15_OFF(%rsp) ;\

#define TRAP_DEF(x)    \
	STORE_ALL_REGS \
	movq $(x), TF_WHICH(%rsp) ;\
	addq $TF_WHICH, %rsp ;\
    jmp default_handler
	
#define TRAP_DEF_NO_ERR_PUSH(x)    \
	pushq $0 ; \
	STORE_ALL_REGS \
	movq $(x), TF_WHICH(%rsp) ;\
	addq $TF_WHICH, %rsp ;\
    jmp default_handler

.globl undefined_handler
undefined_handler:
	jmp .

default_handler:
	movq %rsp, %rdi
	callq dflt_c_handler
	subq $TF_WHICH, %rsp ;\
	iretq

IDTVEC(T_DIVIDE_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_DIVIDE_FAULT)
IDTVEC(T_DEBUG)
	TRAP_DEF_NO_ERR_PUSH(T_DEBUG)
IDTVEC(T_NMI_INT)
	TRAP_DEF_NO_ERR_PUSH(T_NMI_INT)
IDTVEC(T_BKPT_TRAP)
	TRAP_DEF_NO_ERR_PUSH(T_BKPT_TRAP)
IDTVEC(T_OVRFLW_TRAP)
	TRAP_DEF_NO_ERR_PUSH(T_OVRFLW_TRAP)
IDTVEC(T_BOUND_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_BOUND_FAULT)
IDTVEC(T_UNDEF_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_UNDEF_FAULT)
IDTVEC(T_NOMATH_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_NOMATH_FAULT)
IDTVEC(T_DOUBLE_FAULT)
	TRAP_DEF(T_DOUBLE_FAULT)
IDTVEC(T_COPROC_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_COPROC_FAULT)
IDTVEC(T_TSS_FAULT)
	TRAP_DEF(T_TSS_FAULT)
IDTVEC(T_SEG_FAULT)
	TRAP_DEF(T_SEG_FAULT)
IDTVEC(T_STACK_FAULT)
	TRAP_DEF(T_STACK_FAULT)
IDTVEC(T_GP_FAULT)
	TRAP_DEF(T_GP_FAULT)
IDTVEC(T_PAGE_FAULT)
	TRAP_DEF(T_PAGE_FAULT)
IDTVEC(T_RSVD)
	TRAP_DEF(T_RSVD)
IDTVEC(T_MATH_FAULT)
	TRAP_DEF_NO_ERR_PUSH(T_MATH_FAULT)
IDTVEC(T_ALIGN_FAULT)
	TRAP_DEF(T_ALIGN_FAULT)
IDTVEC(T_MACH_ABORT)
	TRAP_DEF_NO_ERR_PUSH(T_MACH_ABORT)
IDTVEC(T_SIMD_FAULT)
	TRAP_DEF(T_SIMD_FAULT)