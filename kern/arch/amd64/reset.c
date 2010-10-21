#include <stdint.h>
#include <bs_commands.h>
#include "idt_amd64.h"
#include "idt_common.h"
#include "inlined_asm_amd64.h"

/* 
 * bw: I originally learned about this through various operating systems and
 * osdev. Finding documentation on how it actually works isn't easy. So this
 * should be considered the backup. I believe a better way is to force the
 * triple fault.
 */
static void *
keyboard_controller_reset() {
	outb(0xFE, 0x64);
	return 0;
}

static void
reset_cmd_help() {
	printf("Reset the damn thing\n");
}

/*
 * bw: This idea came from osdev. Set a 0 length IDT, and then invoke an int.
 */
static void *
zero_length_idt_reset() {
	struct idt_descriptor idtr = {
		.limit = 0,
		.base = 0
	};
	lidt((void *)&idtr);
	__asm__ volatile ("int $0");
	
}

/* Hook this into our debugger */
#ifdef RESET_WITH_8042
BS_COMMAND_DECLARE(reset, keyboard_controller_reset, reset_cmd_help);
#else
BS_COMMAND_DECLARE(reset, zero_length_idt_reset, reset_cmd_help);
#endif