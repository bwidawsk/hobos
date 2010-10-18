#ifndef __IA_DEFINES_H_
#define __IA_DEFINES_H_

#define CR0_PG (1 << 31) /* set: paging enabled */
#define CR0_CD (1 << 30) /* set: caches disabled */
#define CR0_NW (1 << 29) /* set: not write-through */
#define CR0_AM (1 << 18) /* set: automatic alignment checking */
#define CR0_WP (1 << 16) /* set: supervisor write protect pages */
#define CR0_NE (1 << 5) /* set: FPU errors TODO: what??? */
#define CR0_ET (1 << 4) /* set: math coprocessor, sbo */
#define CR0_TS (1 << 3) /* set: delays saving of x87 context on task switch */
#define CR0_EM (1 << 2) /* set: emulate floating point instructions (generates NM exception) */
#define CR0_MP (1 << 1) /* */
#define CR0_PE (1 << 0) /* set: protected mode is enabled */

#define CR3_PCID_MASK (0xFFF)
#define CR3_PCD (1 << 4) /* set: disable caching of page directory */
#define CR3_PWT (1 << 3) /* set: use write through cachine for the page-directory */

#define CR4_PCIDE (1 << 17) /* set: */
#define CR4_SMXE (1 << 14) /* set: enable SMX operations */
#define CR4_VMXE (1 << 13) /* set: enable VMX operations */
#define CR4_OSXMMEXCPT (1 << 10) /* set: OS supports unmasked SIMD FP exceptions */
#define CR4_OSFXSR (1 << 9) /* set: OS supports FXSAVE and FXRSTOR instructions
							* and instructions save XMM and MXCSR registers */
#define CR4_PCE (1 << 8) /* set: enable PMCs */
#define CR4_PGE (1 << 7) /* set: enable global page feature bit 8 of pd or pte */
#define CR4_MCE (1 << 6) /* set: enable Machine Check Exceptions */
#define CR4_PAE (1 << 5) /* set: set to enter IA-32e (or get 36 bits phys addrs) */
#define CR4_PSE (1 << 4) /* set: enable  4 MB pages */
#define CR4_DE  (1 << 3) /* set: disable DR4 and DR5 registers */
#define CR4_TSD (1 << 2) /* set: Disable ring 3 rdtsc */
#define CR4_PVI (1 << 1) /* set: enable virtual interrupt flag */
#define CR4_VME (1 << 0) /* set: use virtual 8086 mod extensions */

#define PT_PAGE_PHYS_MASK (0x000FFFFFFFFFF000UL)

#define PDE_P (1 << 0)
#define PDE_RW (1 << 1)
#define PDE_US (1 << 2)
#define PDE_PWT (1 << 3)
#define PDE_PCD (1 << 4)
#define PDE_A (1 << 5)
#define PDE_D (1 << 6)
#define PDE_PS (1 << 7)
#define PDE_G (1 << 8)
#define PDE_PAT (1 << 12)
#define PDE_XD (1UL << 63UL)

#define PTE_P (1 << 0)
#define PTE_RW (1 << 1)
#define PTE_US (1 << 2)
#define PTE_PWT (1 << 3)
#define PTE_PCD (1 << 4)
#define PTE_A (1 << 5)
#define PTE_D (1 << 6)
#define PTE_PAT (1 << 7)
#define PTE_G (1 << 8)
#define PTE_XD (1UL << 63UL)

#define IA32_EFER_SCE (1 << 0) /* syscall enable */
#define IA32_EFER_LME (1 << 8)
#define IA32_EFER_LMA (1 << 10)
#define IA32_EFER_NXE (1 << 11) /*execute disable bit enabled */
#define IA32_EFER 0xc0000080

#define CPUID_EFER_NXE1 (1 << 20)
#define CPUID_EFER_NXE2 (1 << 29)

#define PAGE_SHIFT			12UL

#define PD_SHIFT 		(21UL)
#define PDE_PER_PAGE (PAGE_SIZE / sizeof(pde_t))
#define PD_BITS 9
#define PT_SHIFT		PAGE_SHIFT
#define PTE_PER_PAGE (PAGE_SIZE / sizeof(pte_t))
#define PT_BITS 12
#endif

