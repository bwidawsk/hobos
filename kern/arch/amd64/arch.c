#include "include/irq.h"
#include "arch/arch.h"

/* TODO: Change this to max cpus */
struct arch_state archbuf[TEMP_MAX_CPU];

void arch_pause() {
	__asm__ volatile ("pause");
}

void arch_from_trapframe(struct arch_state *arch, struct trap_frame64 *tf)
{
	#define TF_A(x) arch->x = tf->tf_##x
	TF_A(rax); TF_A(rbx); TF_A(rcx); TF_A(rdx);
	TF_A(r8); TF_A(r9); TF_A(r10); TF_A(r11);
	TF_A(r12); TF_A(r13); TF_A(r14); TF_A(r15);
	TF_A(rdi); TF_A(rsi); TF_A(rbp); TF_A(rsp);
	#undef TF_A
}

int popcount(uint64_t operand)
{
	int bits;

	__asm__ ("popcnt %1, %0"
			 : "=r" (bits)
			 : "g" (operand)
			 :);

	return bits;
}
