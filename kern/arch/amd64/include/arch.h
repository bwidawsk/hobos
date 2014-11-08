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

/* Capturing architectural state is tricky. Getting the IP and segment registers
 * takes a lot of work with is TODO. All other registers we can pretty easily save
 * off if we are very careful about using stack, and calling functions.
 * The solution we use it to statically allocate a chunk of memory, anduse that for
 * architectural captures.
 *
 * NB: this is probably fragile. Since it's debug only though, that's
 * probably okay.
 */
#define TEMP_MAX_CPU 4
extern struct arch_state archbuf[TEMP_MAX_CPU];
static inline void
capture_arch_state(void)
{
	// TODO: Get the current CPU, const int mycpu = X
	__asm__ volatile(
			 "movq %%rbp, %[rsp];" /* FIXME: this will fail without frame pointers */
			 "movq %%rax, %[rax];"
			 "movq %%rbx, %[rbx];"
			 "movq %%rcx, %[rcx];"
			 "movq %%rdx, %[rdx];"
			 "movq %%r8, %[r8];"
			 "movq %%r9, %[r9];"
			 "movq %%r10, %[r10];"
			 "movq %%r11, %[r11];"
			 "movq %%r12, %[r12];"
			 "movq %%r13, %[r13];"
			 "movq %%r14, %[r14];"
			 "movq %%r15, %[r15];"
			 "movq %%rdi, %[rdi];"
			 "movq %%rsi, %[rsi];"
			 /* Now that registers are saved we can read RBP. RBP is stored on the
			  * stack, and the mov instruction cannot mov from memory, to memory. We
			  * need to use a temp register. r11 is the canonical scratch register
			  * in SVSV (AMD64) ABI
			  */
			 "movq (%%rsp), %%r11;"
			 "movq %%r11, %[rbp];"
			 : [rsp] "=m" (archbuf[0].rsp),
			   [rax] "=m" (archbuf[0].rax),
			   [rbx] "=m" (archbuf[0].rbx),
			   [rcx] "=m" (archbuf[0].rcx),
			   [rdx] "=m" (archbuf[0].rdx),
			   [r8]  "=m" (archbuf[0].r8),
			   [r9]  "=m" (archbuf[0].r9),
			   [r10] "=m" (archbuf[0].r10),
			   [r11] "=m" (archbuf[0].r11),
			   [r12] "=m" (archbuf[0].r12),
			   [r13] "=m" (archbuf[0].r13),
			   [r14] "=m" (archbuf[0].r14),
			   [r15] "=m" (archbuf[0].r15),
			   [rdi] "=m" (archbuf[0].rdi),
			   [rsi] "=m" (archbuf[0].rsi),
			   [rbp] "=m" (archbuf[0].rbp)
			 :
			 :
			 );
}
static inline struct arch_state *
get_arch_state(int cpu)
{
	KASSERT(cpu < TEMP_MAX_CPU, "Invalid CPU\n");
	return &archbuf[cpu];
}

extern struct thread *this_thread(void);
extern void *arch_xlate_pa(void *pa);

struct trap_frame64;
extern void arch_from_trapframe(struct arch_state *arch, struct trap_frame64 *tf);
