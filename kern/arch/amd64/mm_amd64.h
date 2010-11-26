#ifndef __MM_AMD64_H__
#define __MM_AMD64_H__

#define KVA_UPPER_BITS (-1ULL << VA_RSVD_SHIFT)

#define KVADDR(l4, l3, l2, l1) ( \
	((uint64_t)KVA_UPPER_BITS) | \
	((uint64_t)(l4) << PML4_SHIFT) | \
	((uint64_t)(l3) << PDPT_SHIFT) | \
	((uint64_t)(l2) << PD_SHIFT) | \
	((uint64_t)(l1) << PT_SHIFT))

#define UVADDR(l4, l3, l2, l1) ( \
	((uint64_t)(l4) << PML4_SHIFT) | \
	((uint64_t)(l3) << PDPT_SHIFT) | \
	((uint64_t)(l2) << PD_SHIFT) | \
	((uint64_t)(l1) << PT_SHIFT))


extern void *va_base;
#define KVA_TO_PA(va) ((((uint64_t)va) - (uint64_t)&va_base))
#define PA_TO_KVA(va) (((uint64_t)va) + ((uint64_t)&va_base))

#define KERNEL_PML 511ULL
#define DMAP_PML 510ULL
#define RECURSIVE_PML 509ULL

#define DMAP_BASE ((-1ULL << VA_RSVD_SHIFT) | (DMAP_PML << PML4_SHIFT))
#define DMAP_TOP (((-1ULL << VA_RSVD_SHIFT) | ((DMAP_PML + 1ULL) << PML4_SHIFT)) - 1)

#define DMAP_XLATE_PA(pa) \
	(DMAP_BASE + (uint64_t)pa)

#define DMAP_XLATE_VA(va) \
	((uint64_t)va - DMAP_BASE)

#define VA_IN_DMAP(va) \
	((((uint64_t)va) >= DMAP_BASE) && (((uint64_t)va) < DMAP_TOP))

#define VA_TO_PTE(va) (pdpte_t *) \
	(KVADDR(RECURSIVE_PML, 0, 0, 0) + \
		((va >> PT_SHIFT) & (1 << (PML4_BITS + PDPT_BITS + PD_BITS + PT_BITS)) - 1) * sizeof(pte_t))	
	
#define VA_TO_PDE(va) (pdpte_t *) \
	(KVADDR(RECURSIVE_PML, RECURSIVE_PML, 0, 0) + \
		((va >> PD_SHIFT) & (1 << (PML4_BITS + PDPT_BITS + PD_BITS)) - 1) * sizeof(pde_t))	
	
#define VA_TO_PDPTE(va) (pdpte_t *) \
	(KVADDR(RECURSIVE_PML, RECURSIVE_PML, RECURSIVE_PML, 0) + \
		((va >> PDPT_SHIFT) & (1 << (PML4_BITS + PDPT_BITS)) - 1) * sizeof(pdpte_t))
	
#define VA_TO_PML4E(va) (pml4e_t *) \
	(KVADDR(RECURSIVE_PML, RECURSIVE_PML, RECURSIVE_PML, RECURSIVE_PML) + \
		((va >> PML4_SHIFT) & (1 << PML4_BITS) - 1) * sizeof(pml4e_t))

void *arch_xlate_pa(void *pa);
#endif
