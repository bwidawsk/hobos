#include <mm/mm.h>

extern void *arch_xlate_pa(void *);

void *
paddr_to_vaddr(void *pa) {
	return arch_xlate_pa(pa);
}