#include <stdint.h>
#include <timer.h>

TIMER_CREATE_LIST;

volatile uint64_t ttick = 0;

int
periodic_timer_tick(struct timer_driver *timer) {
	atomic_add_64(&ttick, 1);
}

void
timer_init() {
	int throwaway = 0;
	struct timer_driver *timer;

	int set_periodic = 0;
	TIMER_FOREACH(timer, throwaway) {
		if(timer->init != NULL)
			timer->init(timer);

		// TODO: Register the first timer (why not the best?
		if (!set_periodic) {
			register_irq(timer->irq, periodic_timer_tick, timer);
			set_periodic++;
		}
	}
}
