#ifndef __AMD64_DEFINES_H_
#define __AMD64_DEFINES_H_

#define VA_RSVD_SHIFT	(47UL)
#define PML4_SHIFT 		(39UL)
#define PML4E_PER_PAGE (PAGE_SIZE / sizeof(pml4e_t))
#define PML4_BITS 9
#define PDPT_SHIFT 		(30UL)
#define PDPTE_PER_PAGE (PAGE_SIZE / sizeof(pdpte_t))
#define PDPT_BITS 9

#define PML4E_P (1 << 0)
#define PML4E_RW (1 << 1)
#define PML4E_US (1 << 2)
#define PML4E_PWT (1 << 3)
#define PML4E_PCD (1 << 4)
#define PML4E_A (1 << 5)
#define PML4E_PS (1 << 7)
#define PML4E_XD (1UL << 63UL)

#define PDPTE_P (1 << 0)
#define PDPTE_RW (1 << 1)
#define PDPTE_US (1 << 2)
#define PDPTE_PWT (1 << 3)
#define PDPTE_PCD (1 << 4)
#define PDPTE_A (1 << 5)
#define PDPTE_D (1 << 6)
#define PDPTE_PS (1 << 7)
#define PDPTE_G (1 << 8)
#define PDPTE_PAT (1 << 12)
#define PDPTE_XD (1UL << 63UL)

typedef uint64_t pml4e_t;
typedef uint64_t pdpte_t;
typedef uint64_t pde_t;
typedef uint64_t pte_t;

#endif