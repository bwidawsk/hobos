#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "builtins.h"

int popcount(uint64_t operand)
{
    int bits;

    __asm__ ("popcnt %1, %0"
             : "=r" (bits)
             : "g" (operand)
             :);

    return bits;
}

int main()
{
	int i;
	for (i = 0; i < 1ULL<<63; i+=rand()) {
		assert(generic_popcount(i) == __builtin_popcountll(i));
	}

	for (i = 0; i < 1ULL<<63; i+=rand()) {
		assert(popcount(i) == __builtin_popcountll(i));
	}

	return 0;
}
