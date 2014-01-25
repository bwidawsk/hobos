#include <mm/page.h>
#include <mm/page_allocator.h>

static struct mm_page_allocator *page_allocator;

void register_page_allocator(struct mm_page_allocator *pa)
{
	page_allocator = pa;
}

struct mm_page_allocator *get_page_allocator(__attribute__((__unused__))enum allocator_flavor type)
{
	return page_allocator;
}
