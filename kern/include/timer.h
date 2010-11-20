#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_KEY sect_timers

#define TIMER_CREATE_LIST CTLIST_CREATE(TIMER_KEY, struct timer_driver *); 

#define TIMER_DECLARE(timer_name, tinit) \
	static struct timer_driver timer_name = { \
		.name = #timer_name, \
		.init = tinit \
		}; \
	CTLIST_ELEM_ADD(TIMER_KEY, timer_name##_list_ptr, struct timer_driver *, (struct timer_driver *)&timer_name);

/* Gives a pointer to each timer device. */
#define TIMER_FOREACH(elem, garbage) \
	CTLIST_FOREACH(elem, TIMER_KEY, garbage)

struct timer_driver {
	char *name;
	void (*init)(struct timer_driver *);
	int irq;
};

#endif
