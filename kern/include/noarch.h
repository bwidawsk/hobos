#ifndef __NO_ARCH_H__
#define __NO_ARCH_H__

extern void arch_pause();
extern struct thread *this_thread();
extern void bt(void);
extern void bt_fp(void *fp);
extern void *arch_xlate_pa(void *pa);

#endif
