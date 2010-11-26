#ifndef __AMD64_ATOMIC_H__
#define __AMD64_ATOMIC_H__

uint8_t atomic_cmpxchg_64(volatile void *dest, uint64_t expect, uint64_t src);
void atomic_add_64(volatile void *val, uint64_t add_amt);

#endif
