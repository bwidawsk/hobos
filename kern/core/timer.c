#include <stdint.h>
#include <interrupt.h>
#include <timer.h>
#include <noarch.h> // arch_pause
#include <arch/atomic.h> // atomic_add_64

TIMER_CREATE_LIST;

struct timer *main_timer = NULL;
static uint64_t timer_hz = 0;

volatile uint64_t ttick = 0;

int
periodic_timer_tick(void *data) {
	atomic_add_64(&ttick, 1);
	return INTERRUPT_SERVICED;
}

void INITSECTION
timer_init() {
	int throwaway = 0;
	struct timer *timer;

	int did_it = 0;
	TIMER_FOREACH(timer, throwaway) {
		if(timer->init != NULL)
			timer->init(timer);

		// TODO: Register the first timer which has a periodic timer (why not the best?)
		if (!did_it && timer->set_periodic != NULL) {
			register_irq(timer->irq, periodic_timer_tick, timer);
			main_timer = timer;
			did_it++;
		}
	}
}

void
set_system_timer(uint64_t timeout_usec) {
	main_timer->set_periodic(main_timer, timeout_usec);
	timer_hz = USECS_TO_HZ(timeout_usec);
}

/* TODO: create a more granular delay function */
void
timed_delay(uint64_t timeout_usec) {

	// our timer is setup at a specific frequency, we can't
	// do any less than that.
	uint32_t min_usec = HZ_TO_USECS(timer_hz);
	if(timeout_usec < min_usec)
		timeout_usec = min_usec;

	uint64_t start = ttick;
	uint64_t end_tick = ((timer_hz * timeout_usec) / 1000000) + start;

	if (end_tick == start)
		end_tick++;

	while(ttick < end_tick)
		arch_pause();

}
