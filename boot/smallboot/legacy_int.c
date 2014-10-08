#include "legacy_int.h"
int
legacy_int(unsigned char which, struct legacy_regs_32 *regs) {
	unsigned int return_flags = 0;

	// interrupt is pushed, and rflags is returned in the same place
	__asm__ volatile(
		"push	%7;"
		"call	intX;"
		"pop	%0;"
		: "=m" (return_flags), "=a" (regs->eax), "=b" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx),
			"=D" (regs->edi), "=S" (regs->esi)
		: "m" (which), "a" (regs->eax), "b" (regs->ebx), "c" (regs->ecx), "d" (regs->edx),
			"D" (regs->edi), "S" (regs->esi)
	);

	return return_flags;
}
