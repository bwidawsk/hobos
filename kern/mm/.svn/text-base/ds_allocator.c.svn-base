#include <mm/page.h>
#include <mm/page_allocator.h>

#define DS_ALLOC_PRINT(x, ...)

/* dead simple allocator, should not be used for any sort of production */

static void setup_allocator_struct();
static pfn_t ds_get_page(struct mm_page_allocator *pallocator);
static int ds_get_contig_pages(struct mm_page_allocator *pallocator, int count, pfn_t *space);
static void ds_free_page(struct mm_page_allocator *pallocator, pfn_t pfn);
static void ds_free_pages(struct mm_page_allocator *pallocator, int count, pfn_t *pfns);
static void ds_fixup_structures(struct mm_page_allocator *pallocator, void *  (*convert)(void *  orig));
#define MAX_DS_ALLOCATORS 1

struct ds_page_allocator {
	struct mm_page_allocator allocator;
	uint8_t which_allocator;
	pfn_t *page_array;
	uint64_t next_free_pindex;
	uint64_t pa_diff; // difference between 0 and first managed memory location
};

struct ds_page_allocator ds_page_allocators[MAX_DS_ALLOCATORS];

#define DS_ALLOCATOR_NAME "Dead Simple Allocator"

struct mm_page_allocator *
ds_init(memory_region_t *regions, int count) {
	int i = 0;
	uint64_t total_pages = 0;
	uint32_t pages_for_array;
	uint32_t array_size;
	uint64_t top_memory, bottom_memory;
	/* 
	 * Firstly, find out how many pages total we have, and create an array
	 * for those pages with the memory.
	 */
	KASSERT(count == 1); // for now we don't support sparse memory
	do {
		total_pages += (PAGE_FROM_VAL(regions[i].len));
	} while(++i < count);

	array_size = sizeof(pfn_t) * total_pages;
	array_size = ROUND_UP(array_size, PAGE_SIZE);
	pages_for_array = PAGE_FROM_VAL(array_size);

	// round up a page and subtract from total pages
	// below assumes only 1 region
	
	/* This shouldn't be necessary unless BIOS/bootloader sucks */
	top_memory = ROUND_DOWN(regions[0].addr + regions[0].len, PAGE_SIZE);
	bottom_memory = ROUND_UP(regions[0].addr, PAGE_SIZE);
	ds_page_allocators[0].pa_diff = bottom_memory;
	
	/* Since it may be hobos bootloader, let's check */
	KASSERT(top_memory == regions[0].addr + regions[0].len);
	KASSERT(bottom_memory == regions[0].addr);
	
	ds_page_allocators[0].page_array = (pfn_t *)top_memory - PAGE_TO_VAL(pages_for_array);
	setup_allocator_struct();
	ds_page_allocators[0].next_free_pindex = 0;
	ds_page_allocators[0].allocator.total_pages = PAGE_FROM_VAL(top_memory - bottom_memory - pages_for_array);
	ds_page_allocators[0].allocator.used_pages = 0;
	for(i = 0; i < ds_page_allocators[0].allocator.total_pages; i++) {
		ds_page_allocators[0].page_array[i] = PAGE_FROM_VAL(bottom_memory) + i;
	}

	DS_ALLOC_PRINT("ds_allocator set up to manage %x pages, with array at %x\n",
		ds_page_allocators[0].allocator.total_pages, 
		ds_page_allocators[0].page_array);

	return &ds_page_allocators[0].allocator;
}

static void
setup_allocator_struct() {
	ds_page_allocators[0].allocator.get_page = ds_get_page;
	ds_page_allocators[0].allocator.get_contig_pages = ds_get_contig_pages;
	ds_page_allocators[0].allocator.free_page = ds_free_page;
	ds_page_allocators[0].allocator.free_contig_pages = ds_free_pages;
	ds_page_allocators[0].allocator.fixup_structures = ds_fixup_structures;
	ds_page_allocators[0].which_allocator = 0;
	int i = 0;
	const char *temp = DS_ALLOCATOR_NAME;
	while(*temp != '\0') {
		ds_page_allocators[0].allocator.name[i] = *temp;
		temp++;
		i++;
	}
	ds_page_allocators[0].allocator.name[i] = '\0';
}

static pfn_t 
ds_get_page(struct mm_page_allocator *pallocator) { 
	// TODO: not threadsafe
	pfn_t ret = NO_PFN;
	struct ds_page_allocator *allocator = (struct ds_page_allocator *)pallocator;
	ret = allocator->page_array[allocator->next_free_pindex];
	allocator->next_free_pindex++;
	pallocator->used_pages++;
	return ret; 
}

static int
ds_get_contig_pages(struct mm_page_allocator *pallocator, int count, pfn_t *space)  { 
	// TODO: not threadsafe
	struct ds_page_allocator *allocator = (struct ds_page_allocator *)pallocator;
	for(int i = 0; i < count; i++) {
		space[i] = allocator->page_array[allocator->next_free_pindex];
	}
	allocator->next_free_pindex += count;
	pallocator->used_pages += count;
	return 0;
}

static void ds_free_page(struct mm_page_allocator *pallocator, pfn_t pfn) { return; }
static void ds_free_pages(struct mm_page_allocator *pallocator, int count, pfn_t *pfns) { return; }

static void 	
ds_fixup_structures(struct mm_page_allocator *pallocator, void * (*convert)(void * orig)) {
struct ds_page_allocator *allocator = (struct ds_page_allocator *)pallocator;
allocator->page_array = convert(allocator->page_array);

}