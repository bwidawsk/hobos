#ifndef _PAGE_ALLOCATOR_
#define _PAGE_ALLOCATOR_
#define NO_PFN 0
/* 
 * pretty self-explanatory page allocator, one note is add_page is used
 * to free up early pages used by things like bss, which aren't needed
 */
struct mm_page_allocator {
	pfn_t 	(*get_page)(struct mm_page_allocator *);
	int 	(*get_contig_pages)(struct mm_page_allocator *, int count, pfn_t *space);
	void 	(*free_page)(struct mm_page_allocator *, pfn_t pfn);
	void 	(*free_contig_pages)(struct mm_page_allocator *, int count, pfn_t *pfns);
	void 	(*add_page) (struct mm_page_allocator *, pfn_t pfn);
	void 	(*fixup_structures)(struct mm_page_allocator *, void *  (*convert)(void *  orig));
	uint64_t total_pages;
	uint64_t used_pages;
	char name[32];
};

typedef struct {
	uint32_t rsvd; /* here to match multiboot */
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) memory_region_t ;


/*
 * init functions must expect to be called with a 1:1 page mapping 
 * (or no page tables), and then later get a call to fix up things. This
 * call is called fixup_structures, and it takes as an argument a function
 * pointer will do the correct mapping
 */
struct mm_page_allocator *ds_init(memory_region_t *regions, int count);
#endif