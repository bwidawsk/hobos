#include <mm/mm.h>

extern void *arch_xlate_pa(void *);
extern void *arch_xlate_va(void *);

void *
paddr_to_vaddr(void *pa) {
	return arch_xlate_pa(pa);
}

void *
vaddr_to_paddr(void *va) {
	return arch_xlate_va(va);
}
