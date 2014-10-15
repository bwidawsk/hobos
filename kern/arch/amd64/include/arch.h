typedef uint64_t register_t;

struct arch_state {
	register_t rax;
	register_t rbx;
	register_t rcx;
	register_t rdx;
	register_t r8;
	register_t r9;
	register_t r10;
	register_t r11;
	register_t r12;
	register_t r13;
	register_t r14;
	register_t r15;
	register_t rdi;
	register_t rsi;
	register_t rbp;
	register_t rsp;
	register_t rflags;
	/* FPU state */
	/* AVX state */
};

struct thread *this_thread(void);
void *arch_xlate_pa(void *pa);
