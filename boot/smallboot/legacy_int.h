#ifndef _LEGACY_INT_H_
#define _LEGACY_INT_H_
#include "stdint.h"

struct legacy_regs_32 {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t esi;
	uint32_t edi;
};

int legacy_int(unsigned char which, struct legacy_regs_32 *regs);

#endif