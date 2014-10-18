#include <console.h>

void arch_pause(void) __attribute__((weak, alias ("__noarch_pause")));
static void __noarch_pause(void) { }

void bt(void) __attribute__((weak, alias ("__noarch_bt")));
void __noarch_bt(void) { printf("No backtrace information available\n"); }

void bt_fp(void *fp) __attribute__((weak, alias ("__noarch_bt_fp")));
void __noarch_bt_fp(void *fp) { printf("No backtrace information available\n"); }

