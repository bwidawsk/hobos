#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define yesno(x) (x) ? "yes" : "no"

void decode_pde(uint64_t pde)
{
	printf("Read access: %s\n", yesno(pde & 1));
	printf("Write access: %s\n", yesno((pde >> 1) & 1));
	printf("Execute access: %s\n", yesno((pde >> 2) & 1));

	if ((pde >> 7) & 1) {
		printf("2M PDE\n");
		printf("\tEPT memory type: %ld\n", (pde >> 3) & 7);
		printf("\tIgnore PAT: %s\n", yesno((pde >> 6) & 1));
		printf("\tPhysical range: 0x%lx-0x%lx\n",
			   pde & ~0xFFFF0000000FFFFF,
			   pde + (2<<20) & ~0xFFFF0000000FFFFF);
	}

}

void decode(char *argv[])
{
	if (!strcmp(argv[0], "pde")) {
		return decode_pde(strtoull(argv[1], NULL, 0));
	}
}

int main(int argc, char *argv[]) {

	if (argc > 2) {
		decode(&argv[1]);
		return 0;
	}

	uint64_t addr = strtoull(argv[1], NULL, 0);
	// assume 2M pages for now
	uint16_t pml4 = (addr >> 39UL) & 0x1FFUL;
	uint16_t dirptr = (addr >> 30UL) & 0x1ffUL;
	uint16_t dir = (addr >> 21UL) & 0x1ffUL;
	printf("pml4 = 0x%x\ndirptr = 0x%x\ndir = 0x%x\n\n", pml4, dirptr, dir);
	printf("cr3[0x%x]\n", pml4 << 3);
	printf("pml4[0x%x]\n", dirptr << 3);
	printf("pdpt[0x%x]\n", dir << 3);
	uint32_t offset;
	if (1) {
		uint16_t pt = (addr >> 12UL) & 0x1ffUL;
		printf("pdp[0x%x]\n", pt << 3);
		printf("pt[0x%x]\n", offset << 3);
		offset = (addr & 0xFFFUL);
	} else /* 2M pdes */
		offset = (addr & 0xFFFFFUL);
	printf("offset = 0x%x\n", offset);
	return 0;
}
