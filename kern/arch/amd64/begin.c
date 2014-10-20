#include <mm/mm.h>
#include "mm_amd64.h"
#include <mm/page.h>
#include <mm/page_allocator.h>
#include "gdt.h" // for the GDT stuff TODO: move GDT stuff to it's own file
#include "tss_amd64.h" // for temporary task stuff TODO: move this too
#include "include/asm.h"
#include "amd64_defines.h"
#include "ia_defines.h"
#include "idt_common.h"
#include "multiboot.h"
#define __ELF_WORD_SIZE 64
#include "elf.h"

extern void *kernel_load_start;
extern void *kernel_load_end;

void amd64_begin(struct multiboot_info *mboot_info, uint32_t magic)  __attribute__((noreturn));
static void new_beginning();
static uint8_t temp_interrupt_stack[4096] _INITSECTION_;
static uint64_t kernel_size;
static void *stack_va;
// 20 is Random
#define MAX_BOOT_MMAP_ENTRIES 20
struct multiboot_mmap_entry copied_map[MAX_BOOT_MMAP_ENTRIES];
multiboot_elf_section_header_table_t copied_elf_header;

struct segment_descriptor mboot_gdt[NUM_SEGMENTS]
	__attribute__((section(".multiboot"))) = {
		GDT_SEGMENTS
	};

struct segment_descriptor gdt [NUM_SEGMENTS] = {
	GDT_SEGMENTS
	};
static uint8_t use_xd = 0;
/* Do stuff and set flags based on values read in CPU id. */
void
cpuid_setup() {
	#define CPUID_RET_EAX 0
	#define CPUID_RET_EBX 0
	#define CPUID_RET_ECX 0
	#define CPUID_RET_EDX 0
	cpuid_return_t ret;
	cpuid(0x80000001, ret);
	if (ret[CPUID_RET_EDX] & CPUID_EFER_NXE1 ||
		ret[CPUID_RET_EDX] & CPUID_EFER_NXE2) {
		/* TODO:
		movl $IA32_EFER, %ecx
		rdmsr
		or $IA32_EFER_NXE, %eax
		wrmsr
		use_xd++;
		*/
	}
	/* strcmp
	if (ret[CPUID_RET_EBX] != "Genu" ||
		ret[CPUID_RET_ECX] != "ntel" ||
		ret[CPUID_RET_EDX] != "ineI") {
		highest_eax = ret[CPUID_RET_EAX];
	} else {
		return;
	}
	*/
}

struct tss_64 task;

/* TODO: put this elsewhere, rename */
void
temp_task_load() {
	volatile struct tss_descriptor *tss;
	tss	= (volatile struct tss_descriptor *)((uint64_t)&gdt[KERNEL_TSS_IDX]);
	tss->base23_0 = (((uint64_t)&task) & 0xFFFFFFUL);
	tss->base63_24 = (((uint64_t)&task) >> 24UL);
	//__asm__ volatile("ltr %0" : : "r" ((uint16_t)KERNEL_TSS));
	ltr((uint16_t)KERNEL_TSS);
}
void
first_temp_task_load() {
	volatile struct tss_descriptor *tss;
	tss	= (volatile struct tss_descriptor *)((uint64_t)&mboot_gdt[KERNEL_TSS_IDX]);
	tss->base23_0 = (((uint64_t)&task) & 0xFFFFFFUL);
	tss->base63_24 = (((uint64_t)&task) >> 24UL);
	//__asm__ volatile("ltr %0" : : "r" ((uint16_t)KERNEL_TSS));
	ltr((uint16_t)KERNEL_TSS);
}

struct mm_page_allocator *primary_allocator;

void
carve_mboot_memory(struct multiboot_mmap_entry *map, int num_entries) {
	printf("Printing memory map passed by bootloader\n");
	int best_entry = 0;
	int usable_sections = 0;
	multiboot_uint64_t maxlen = 0;
	do {
		if (map[num_entries].type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		usable_sections++;
		if (map[num_entries].len > maxlen) {
			best_entry = num_entries;
			maxlen = map[num_entries].len;
		}
		printf("0x%08x  0x%08x\n", map[num_entries].addr, map[num_entries].len);
	} while(num_entries--);

	if (usable_sections >= 3) {
		printf("this looks like sparse memory, results may vary\n");
	}

	// 16 MB is an arbitrary size which we consider "good"
	if (map[best_entry].len <= (1 << 24)) {
		printf("free memory seems small %x\n", map[best_entry].len);
	}

	// start a list of things we need memory for
	//TODO: for now assume all our crap fits in 1 MB
	map[best_entry].addr+=ROUND_UP(kernel_size, PAGE_SIZE);
	map[best_entry].len-=ROUND_UP(kernel_size, PAGE_SIZE);

	#ifdef DS_ALLOCATOR
	primary_allocator = (struct mm_page_allocator *)
		ds_init((memory_region_t *)&map[best_entry], 1);
	#else
	#error no supported allocator selected
	#endif
	printf("%s initialized\n", primary_allocator->name);
	// in theory we could grab a handle to another allocator here and give
	// it a different or sub region of memory.
}

// TODO: this assumes 1 gb page support
#ifndef GB_PAGES
pml4e_t *pml4_phys;
pdpte_t *dmap_pdpt_phys;
pde_t *dmap_pagedir_phys;
pdpte_t *kernel_pdpt_phys;
#define MAX_KERNSIZE_GB 1
pde_t *kernel_pd_phys;
#else
#error no 1GB Page support yer
#endif


/* TODO: get rid of all the hardcoding
bwidawsk@snipes /home/bwidawsk/workspace/C/hobos_trunk/tools $ ./pt_walk 0xffffffff810043d5
pml4 = 0x1ff
dirptr = 0x1fe
dir = 0x8
offset = 0x43d5
cr3[0xff8]
pml4[0xff0]

*/
static void
change_pages_and_jump(void (*func)) {
	pfn_t temp;
	int pdpte, pde;

	// Allocate our page table pages directly from the page allocator we
	// initialized. We do this because we can't reliably translate a VA from
	// into the PA we need.
	temp = primary_allocator->get_page(primary_allocator);
	pml4_phys = (pml4e_t *)PAGE_TO_VAL(temp);
	temp = primary_allocator->get_page(primary_allocator);
	kernel_pdpt_phys = (pdpte_t *)PAGE_TO_VAL(temp);
	temp = primary_allocator->get_page(primary_allocator);
	dmap_pdpt_phys = (pdpte_t *)PAGE_TO_VAL(temp);

	// TODO: we assume 2 pages is enough to hold the dmap pages
	// TODO: we assume allocator gives ascending continuous page frames
	pfn_t pd_pages[MAX_KERNSIZE_GB];
	primary_allocator->get_contig_pages(primary_allocator, MAX_KERNSIZE_GB, pd_pages);
	kernel_pd_phys = (pde_t *)PAGE_TO_VAL(pd_pages[0]);

	// The dmap pages would use a lot of stack if it's a local, so allocate
	// some pages for it, and free it when we finish.
	pfn_t dmap_temp_pages[2];
	primary_allocator->get_contig_pages(primary_allocator, 2, dmap_temp_pages);
	pfn_t *dmap_pages = (pfn_t *)PAGE_TO_VAL(dmap_temp_pages[0]);
	// Use the pages we got to store our real page table pages
	primary_allocator->get_contig_pages(primary_allocator, DMAP_GBS, dmap_pages);
	dmap_pagedir_phys = (pde_t *)PAGE_TO_VAL(dmap_pages[0]);

	for(pde = 0; pde < 512 * DMAP_GBS; pde++) {
		dmap_pagedir_phys[pde] = (pde * (1 << 21)) | PDPTE_P | PDPTE_RW | PDPTE_PS;
	}

	for(pde = 0; pde < 512 * MAX_KERNSIZE_GB; pde++) {
		kernel_pd_phys[pde] = (pde * (1 << 21)) | PDPTE_P | PDPTE_RW | PDPTE_PS;
	}

	for(pdpte = 0; pdpte < 512; pdpte++) {
		dmap_pdpt_phys[pdpte] = ((uint64_t)dmap_pagedir_phys)  | PDE_P | PDE_RW;
	}

	kernel_pdpt_phys[0x1fe] = ((uint64_t)kernel_pd_phys) | PDE_P | PDE_RW;
	#if MAX_KERNSIZE_GB > 1
	#error we dont support > 1 yet
	#endif

	pml4_phys[RECURSIVE_PML] = ((uint64_t)pml4_phys) | PML4E_P | PML4E_RW;

	pml4_phys[DMAP_PML] = ((uint64_t)dmap_pdpt_phys) | PML4E_P | PML4E_RW;
	if (use_xd)
		pml4_phys[DMAP_PML] |= PML4E_XD;

	pml4_phys[KERNEL_PML] = ((uint64_t)kernel_pdpt_phys) | PML4E_P | PML4E_RW;

	primary_allocator->free_page(primary_allocator, dmap_temp_pages[0]);
	primary_allocator->free_page(primary_allocator, dmap_temp_pages[1]);

	temp = primary_allocator->get_page(primary_allocator);
	stack_va = (void *)DMAP_XLATE_PA((void *)PAGE_TO_VAL(temp));

	// We're going to put our thread struct at the top of the stack
	stack_va -= sizeof(struct thread *);

	__asm__ volatile (
		"mov %0, %%cr3\n\t"
		"movq %1, %%rsp\n\t"
		"callq *%%rax\n\t"
		:
		: "r" (pml4_phys), "r" (stack_va), "a" (func)
	);
}

/*
 * When we get here, we are running in 64bit mode with the page tables defined
 * in early_pages.c. This allows us to do some really early stuff in 64bit mode
 * before we actually move to kernel page tables. We can free these early pages
 * after boot, but they don't occupy much space.
 * The function has a few key goals:
 * switch the GDT to our kernel linked address
 * carve our memory and initialize the page allocators
 * set up the page tables which we intend to run on the rest of the time.
 * anything else?
 * TODO: UPDATE: tried the stack, haven't tested
  * until we switch our page tables, exceptions aren't handled by the OS
 * so debugging this part is difficult. I think we can fix this if we just
 * statically allocate a page for the interrupt stack early. Later on we
 * can allocate more pages for it if needed.
 */
void
amd64_begin(struct multiboot_info *mboot_info, uint32_t magic) {
	int i;
	multiboot_uint32_t count;
	struct multiboot_mmap_entry *map;
	static volatile int wait = 0;
#ifdef HALT_ON_ENTRY
	wait = 1;
#endif
	while (wait);

	/* Setup the exception handlers as early as possible to catch errors */
	setup_exception_handlers();
	task.ist1 = (uint64_t)temp_interrupt_stack;
	first_temp_task_load();
	cpuid_setup();

	/* It is unsafe to print anything until here */

	struct gdt_descriptor64 gdtdesc = {
		.base = (uint64_t)&gdt,
		.limit = sizeof(mboot_gdt) - 1
	};
	lgdt(&gdtdesc);

	kernel_size = &kernel_load_end - &kernel_load_start;
	map	= (struct multiboot_mmap_entry *)(uint64_t)mboot_info->mmap_addr;

	count = mboot_info->mmap_length/sizeof(struct multiboot_mmap_entry);
	KASSERT(count < MAX_BOOT_MMAP_ENTRIES, (""));
	for(i = 0; i < count; i++) {
		copied_map[i] = map[i];
	}
	copied_map[i].type = 0xbeef;
	carve_mboot_memory(map, count);

	if (mboot_info->flags & MULTIBOOT_INFO_ELF_SHDR) {
		KASSERT(mboot_info->u.elf_sec.size == sizeof(Elf64_Shdr),
				"Bootloader size, and compile time size of elf do not match\n");

		copied_elf_header = mboot_info->u.elf_sec;
	}

	change_pages_and_jump(new_beginning);
	while(1);
}

extern void mi_begin(struct multiboot_mmap_entry *,
					 multiboot_elf_section_header_table_t *,
					 struct mm_page_allocator *);
extern struct thread first_thread;

struct thread *
this_thread() {
	struct thread *td;
	__asm __volatile("movq %%gs:0,%0" : "=r" (td));
	return td;
}

static void
new_beginning() {
	pfn_t temp;
	// The very first thing to do is fix up our allocators so they know about
	// the address space change.
	primary_allocator->fixup_structures(primary_allocator, arch_xlate_pa);
	temp = primary_allocator->get_page(primary_allocator);
	task.ist1 = DMAP_XLATE_PA(PAGE_TO_VAL(temp));
	temp_task_load();
	// Until this point, exceptions won't be handled, see TODO in amd64_begin
	// The only thing we want to do before initializing our consoles is set up
	// malloc so the console/lib code can behave normally

	*(uint64_t *)stack_va = (uint64_t)&first_thread;
	wrmsr(0xC0000102, (uint64_t)stack_va);
	swapgs();

	mi_begin(copied_map, &copied_elf_header, primary_allocator);
}
