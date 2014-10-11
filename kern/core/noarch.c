#include <thread.h>

static void
__noarch_pause(void)
{

}

static struct thread *
__this_thread(void)
{
	panic("Architecture must define a __this_thread\n");
	return NULL;
}

void __noarch_bt(void)
{
	printf("No backtrace information available\n");
}

void __noarch_bt_fp(void *fp)
{
	printf("No backtrace information available\n");
}

void arch_pause(void) __attribute__((weak, alias ("__noarch_pause")));
struct thread *this_thread(void) __attribute__((weak, alias ("__this_thread")));
void bt(void) __attribute__((weak, alias ("__noarch_bt")));
void bt_fp(void *fp) __attribute__((weak, alias ("__noarch_bt_fp")));
