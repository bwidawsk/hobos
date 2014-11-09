#ifndef __NO_ARCH_H__
#define __NO_ARCH_H__

extern void arch_pause();
extern void backtrace_now(void);
extern void backtrace(void *fp);
extern int popcount(uint64_t operand);

#endif
