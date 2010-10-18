#include <stdint.h>
/* if rax == dest
	dest = src
else
	rax = dest
*/
uint8_t
atomic_cmpxchg_64(volatile void *dest, uint64_t expect, uint64_t src)
{
	uint8_t ret = 0;
	uint64_t out_rax;
	__asm__ volatile ("lock cmpxchgq %[src], %[dest] ;"
	"sete %0"
	:  "=g" (ret), "=a" (out_rax)
	: "a" (expect), [src] "r" (src), [dest] "m" (*(uint64_t *)dest)
	);
	return ret;
}

void
atomic_set_64(volatile void *val, uint64_t new_val) {

}