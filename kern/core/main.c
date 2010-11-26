#include <stdint.h>
#include <mm/malloc.h>
#include <console.h>
#include <mutex.h>
#include <multiboot.h>
#include <noarch.h>
#include <thread.h>
#include <timer.h>
#include <dev/block/ata_block.h> // ata_scan_devs, TODO: remove?

/* This was the allocator used/setup by the arch specific code */
struct mm_page_allocator *primary_allocator;

struct thread first_thread = {
	.tid = 0,
	.debug = "poop"
};
extern char version[];

/* FIXME: remove these */
extern void pic8259_init(int);
extern volatile uint64_t ttick;


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
	
	// interrupt setup (needed for when we enumerate)
	// TODO: abstract PIC
	pic8259_init(3);
	timer_init();
	// set up a tickrate for 100HZ
	set_system_timer(HZ_TO_USECS(100));

	pic8259_unmask(0);

	// TODO abstract sti
	__asm__ volatile("sti");
	ata_scan_devs();
	printf("waiting 5 seconds\n");
	timed_delay(5000000);
	__asm__ volatile("ud2");

	while(1) {
		arch_pause();
	}
}
