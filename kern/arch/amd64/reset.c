#include <stdint.h>
#include <bs_commands.h>
#include "inlined_asm_amd64.h"

static void *
keyboard_controller_reset() {
	outb(0xFE, 0x64);
	return 0;
}

static void
reset_cmd_help() {
	printf("Reset the damn thing\n");
}


BS_COMMAND_DECLARE(reset, keyboard_controller_reset, reset_cmd_help);