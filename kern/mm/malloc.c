#include <mm/mm.h>
#include <mm/page.h>
#include <mm/page_allocator.h>
#include <init_funcs.h>

#define EARLY_MALLOC_INITED (1<<0)
#define MALLOC_INITED		(1<<1)
static int malloc_inited = 0;
struct mm_page_allocator *page_allocator;

void
init_early_malloc(struct mm_page_allocator *allocator) {
	page_allocator = allocator;
	malloc_inited |= EARLY_MALLOC_INITED;
}


INITFUNC_DECLARE(malloc_init, INITFUNC_ALLOCATION_SUBSYSTEM) {
	KASSERT(malloc_inited & EARLY_MALLOC_INITED, ("Early malloc wasn't done"));

	/* Call back to the core to get our possibly different page allocator */
	page_allocator = get_page_allocator(page_allocator_default);
}

/*
 * Simple malloc just gets a page from the page allocator and uses the DMAP
 * functions to translate it to a good VA.
 * TODO: heap based allocation.
 */
void *
simple_malloc(uint64_t size) {
	KASSERT(malloc_inited != 0, ("Malloc not inited"));
	pfn_t temp;
	temp = page_allocator->get_page(page_allocator);
	return paddr_to_vaddr((void *)PAGE_TO_VAL(temp));
}

void
simple_free(const void *addr) {
	KASSERT(malloc_inited != 0, ("Malloc not inited"));
	KASSERT((((uint64_t)addr) & PAGE_OFFSET_MASK) == 0, (""));

	void *va = vaddr_to_paddr((void *)addr);

	// TODO: 64 bit hardcode
	uint64_t free_addr = (uint64_t)va;

	page_allocator->free_page(page_allocator, PAGE_FROM_VAL(free_addr));
}
