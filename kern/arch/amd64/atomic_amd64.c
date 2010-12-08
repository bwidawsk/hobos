/* if rax == dest
	dest = src
else
	rax = dest
*/
uint8_t
atomic_cmpxchg_64(volatile void *dest, uint64_t expect, uint64_t src) {
	uint8_t ret = 0;
	uint64_t out_rax = 0;
	__asm__ volatile ("lock cmpxchgq %[src], %[dest] ;"
	"sete %0"
	:  "=g" (ret), "=a" (out_rax)
	: "a" (expect), [src] "r" (src), [dest] "m" (*(uint64_t *)dest)
	);
	return ret;
}

void
atomic_add_64(volatile void *val, uint64_t add_amt) {
	__asm__ volatile ("lock addq %0, %1 ;"
	:
	: "r" (add_amt), "m" (*(uint64_t *)val)
	);
}
