#include <stdint.h>
#include <mm/mm.h>
#include <mm/page.h>
#include <mm/page_allocator.h>

static int malloc_inited = 0;
struct mm_page_allocator *page_allocator;

void 
init_malloc(struct mm_page_allocator *allocator) {
	page_allocator = allocator;
	malloc_inited = 1;
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
simple_free(uint64_t addr) {
	KASSERT(malloc_inited != 0, ("Malloc not inited"));
	KASSERT(addr & PAGE_MASK == 0, ());

	page_allocator->free_page(page_allocator, vaddr_to_paddr(PAGE_FROM_VAL(addr)));
}
