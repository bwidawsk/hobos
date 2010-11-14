#include <stdint.h>
#include <mm/page.h>
#include <mm/page_allocator.h>
#include <mm/malloc.h>
#include <console.h>
#include <mutex.h>
#include <multiboot.h>
#include <noarch.h>
#include <thread.h>

/* This was the allocator used/setup by the arch specific code */
struct mm_page_allocator *primary_allocator;

struct thread first_thread = {
	.tid = 0,
	.debug = "poop"
};
extern char version[];

/* FIXME: remove these */
extern void pit_test();
extern void pic8259_init(int);

/*
 * Machine independent beginning. The machine dependent code can optionally
 * pass in an allocator it may have used early in boot.
 * We also want the memory map so that we can set up our own allocators
 */
void 
mi_begin(struct multiboot_mmap_entry *copied_map, struct mm_page_allocator *primary_allocator) {
	KASSERT(primary_allocator != NULL, ("can't handle null allocator yet\n"));
	init_malloc(primary_allocator);
	console_init();
	printf("Link time = %s\n", version);
	//printf("%p\n", this_thread());
	//printf("%s\n", this_thread()->debug);
	// platform extended features
	// set gsbase
	
	// interrupt setup (needed for when we enumerate)
	
	// platform_enumeration()
	pic8259_init(3);
	pit_test();
	__asm__ volatile("ud2");

	while(1) {
		arch_pause();
	}
}
