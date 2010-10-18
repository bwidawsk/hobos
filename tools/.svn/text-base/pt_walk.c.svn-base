#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
	uint64_t addr = strtoull(argv[1], NULL, 0);
	// assume 2M pages for now
	uint16_t pml4 = (addr >> 39UL) & 0x1FFUL;
	uint16_t dirptr = (addr >> 30UL) & 0x1ffUL;
	uint16_t dir = (addr >> 21UL) & 0x1ffUL;
	uint16_t pt = (addr >> 12UL) & 0x1ffUL;
	uint32_t offset = (addr & 0xFFFUL);
	printf("pml4 = 0x%x\ndirptr = 0x%x\ndir = 0x%x\noffset = 0x%x\n",
		pml4, dirptr, dir, offset);
	printf("cr3[0x%x]\n", pml4 << 3);
	printf("pml4[0x%x]\n", dirptr << 3);
	printf("pdpt[0x%x]\n", dir << 3);
	printf("pdp[0x%x]\n", pt << 3);
	printf("pt[0x%x]\n", offset << 3);
	return 0;
}
