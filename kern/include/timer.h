#ifndef __TIMER_H__
#define __TIMER_H__

#define USECS_TO_HZ(usecs) (1000000/usecs)
#define HZ_TO_USECS(hz)		(1000000/hz)

#define TIMER_KEY sect_timers

#define TIMER_CREATE_LIST CTLIST_CREATE(TIMER_KEY, struct timer *); 

#define TIMER_DECLARE(timer_name, tinit) \
	static struct timer timer_name = { \
		.name = #timer_name, \
		.init = tinit \
		}; \
	CTLIST_ELEM_ADD(TIMER_KEY, timer_name##_list_ptr, struct timer *, (struct timer *)&timer_name);

/* Gives a pointer to each timer device. */
#define TIMER_FOREACH(elem, garbage) \
	CTLIST_FOREACH(elem, TIMER_KEY, garbage)

struct timer {
	char *name;
	void (*init)(struct timer *);
	int (*set_oneshot)(struct timer *, uint64_t usec);
	int (*set_periodic)(struct timer *, uint64_t usec_period);
	int irq;
};

void INITSECTION timer_init();
void set_system_timer(uint64_t timeout_usec);
void timed_delay(uint64_t timeout_usec);

#endif
