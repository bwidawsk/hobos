#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define INTERRUPT_SERVICED 1
#define INTERRUPT_DEFERRED 2
#define INTERRUPT_WTF 4

int register_irq(int vector, int (*handler)(void *), void *data);
void unregister_irq(int vector);

#endif
