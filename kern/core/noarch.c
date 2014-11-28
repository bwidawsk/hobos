#include <console.h>
#include <arch/arch.h>
#include <lib.h>
#include "builtins.h" // generic_popcount()

void arch_pause(void) __attribute__((weak, alias ("__noarch_pause")));
static void __noarch_pause(void) { }

void backtrace_now(void) __attribute__((weak, alias ("__noarch_backtrace_now")));
void __noarch_backtrace_now(void) { printf("No backtrace information available\n"); }

void backtrace(void *fp) __attribute__((weak, alias ("__noarch_backtrace")));
void __noarch_backtrace(void *fp) { printf("No backtrace information available\n"); }

int popcount(uint64_t operand) __attribute__((weak, alias ("__noarch_popcount")));
int __noarch_popcount(uint64_t operand)
{
	return generic_popcount(operand);
}


#include <bs_commands.h>
static void *
do_bt_command(struct console_info *info, int argc, char *argv[])
{
	if (!info)
		backtrace_now();
	else
		backtrace((void *)info->arch_state->rbp);
	return NULL;
}

static void
bt_help()
{
	printf("Show backtrace\n");
}

BS_COMMAND_DECLARE(bt, do_bt_command, bt_help);
