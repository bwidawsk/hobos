#include <mm/malloc.h>
#include <console.h>
#include <mutex.h>
#include <multiboot.h>
#include <noarch.h>
#include <thread.h>
#include <timer.h>
#include <init_funcs.h>
#include <dev/pic/pic.h>
#include <device.h>
#include <fs/vfs.h>

#include "syms.h"

/* This was the allocator used/setup by the arch specific code */
struct mm_page_allocator *primary_allocator;

struct thread first_thread = {
	.tid = 1,
	.debug = "Kernel Thread"
};
extern char version[];

/* FIXME: remove these */
extern struct pic_dev pic_8259;

/*
 * Machine independent beginning. The machine dependent code can optionally
 * pass in an allocator it may have used early in boot.
 * We also want the memory map so that we can set up our own allocators
 */
void mi_begin(struct multiboot_mmap_entry *copied_map,
			  multiboot_elf_section_header_table_t *mboot_elf,
			  struct mm_page_allocator *primary_allocator)
{
	KASSERT(primary_allocator != NULL, ("can't handle null allocator yet\n"));

	init_early_malloc(primary_allocator);
	console_init();
	printf("Link time = %s\n", version);

	get_elf_symbols_mboot(mboot_elf);

#if 0
	printf("%p\n", this_thread());
	printf("%s\n", this_thread()->debug);
#endif

	// interrupt setup (needed for when we enumerate)
	// TODO: abstract PIC
	pic_8259.init(3);
	timer_init();
	// set up a tickrate for 100HZ
	set_system_timer(HZ_TO_USECS(100));

	pic_8259.unmask(0);

	// TODO abstract sti
	__asm__ volatile("sti");
	call_initialization_functions();

	printf("waiting 5 seconds\n");
	timed_delay(5000000);
	__asm__ volatile("ud2");

	while(1) {
		arch_pause();
	}
}
