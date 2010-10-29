#include <arch/arch.h>

struct thread {
	uint64_t tid;
	// arch context
	// soft context
	uint8_t stack[4096];
	struct arch_state arch_state;
	char *debug;
};