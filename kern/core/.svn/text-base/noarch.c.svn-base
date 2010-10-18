#include <stdint.h>
#include <thread.h>

static void
__arch_pause() {

}

static struct thread *
__this_thread() {
	return 0;
}

void arch_pause() __attribute__((weak, alias ("__arch_pause")));
struct thread *this_thread() __attribute__((weak, alias ("__this_thread")));