#include <stdint.h>
#include "amd64_defines.h"
#include "mm_amd64.h"

void *
arch_xlate_pa(void *pa) {
	return (void *)DMAP_XLATE_PA(pa);
}

/* TODO: support non-DMAP */
void *
arch_xlate_va(void *va) {
	KASSERT(VA_IN_DMAP(va), ("Translating an untranslatable address\n"));
	return (void *)DMAP_XLATE_VA(va);
}
