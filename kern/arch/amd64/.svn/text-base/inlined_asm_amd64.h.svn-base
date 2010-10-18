#ifndef __INLINED_ASM_H_
#define __INLINED_ASM_H_

typedef uint32_t cpuid_return_t[4];

static inline void 
outb(uint8_t byte, uint16_t port) {
	__asm__ volatile("outb %0, %1" : : "a" (byte), "d" (port));
}

static inline unsigned char 
inb(uint16_t port) {
	uint8_t retval;
	__asm__ volatile ("inb %1, %0" : "=a"(retval) : "Nd"(port));
	return retval;
}

static inline void
cpuid(uint32_t eax, cpuid_return_t retval) {
	__asm__ volatile (
		"cpuid" 
		: "=a" (retval[0]), "=b" (retval[1]), "=c" (retval[2]), "=d"(retval[3])
		: "a" (eax)
		);
}

static inline void
cpuid_cx(uint32_t eax, uint32_t ecx, cpuid_return_t retval) {
	__asm__ volatile (
		"cpuid" 
		: "=a" (retval[0]), "=b" (retval[1]), "=c" (retval[2]), "=d"(retval[3])
		: "a" (eax), "c" (ecx)
		);
}
static inline void 
ltr(uint16_t which) {
	__asm__ volatile("ltr %0" : : "r" (which));
}

inline void 
write_cr0(uint32_t newcr0) {
	__asm__ volatile ("movl %0, %%cr0" : : "g" (newcr0));
}

inline uint32_t read_cr0() {
	uint32_t retval;
	__asm__ volatile ("movl %%cr0, %0" : "=g" (retval));
	return retval;
}

#if 0
static inline void 
write_cr3(uint32_t newcr3) {
	__asm__ volatile ("mov %0, %%cr3" : : "r" (newcr3));
}
#else
#define write_cr3(newcr3) __asm__ volatile ("mov %0, %%cr3" : : "r" (newcr3))
#endif

static inline uint64_t read_cr2() {
	uint64_t retval;
	__asm__ volatile ("movq %%cr2, %0" : "=r" (retval));
	return retval;
}

static inline uint32_t read_cr3() {
	uint32_t retval;
	__asm__ volatile ("movl %%cr3, %0" : "=g" (retval));
	return retval;
}

static inline void write_cr4(uint32_t newcr4) {
	__asm__ volatile ("movq %0, %%cr4" : : "r" ((uint64_t)newcr4));
}

static inline uint32_t read_cr4() {
	uint64_t retval;
	__asm__ volatile ("movq %%cr4, %0" : "=r" (retval));
	return (uint32_t)retval;
}

static inline void write_cr8(uint32_t newcr8) {
	__asm__ volatile ("movl %0, %%cr8" : : "g" (newcr8));
}

static inline uint32_t read_cr8() {
	uint32_t retval;
	__asm__ volatile ("movl %%cr8, %0" : "=g" (retval));
	return retval;
}

static inline void wrmsr(uint32_t which, uint64_t val) {
	__asm__ volatile ("wrmsr" : : "a" ((uint32_t)val), "d"((uint32_t)(val >> 32)), "c" (which));
}

static inline uint64_t rdmsr(uint32_t which) {
	uint32_t eax, edx;
	__asm__ volatile ("rdmsr" : "=a" (eax), "=d"(edx) : "c" (which));
	return ((uint64_t)edx << 32) | eax;
}

static inline void
swapgs() {
	__asm__ volatile ("swapgs");
}

#endif