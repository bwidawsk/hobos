#include <timer.h>

TIMER_CREATE_LIST;

int
timer_tick(struct timer_driver *timer) {

}

void
timer_init() {
	int throwaway = 0;
	struct timer_driver *timer;

	TIMER_FOREACH(timer, throwaway) {
		if(timer->init != NULL)
			timer->init(timer);

		 register_irq(timer->irq, timer_tick, timer);
	}
}
