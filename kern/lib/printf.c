#include <stdarg.h>
#include <console.h>

extern int kvprintf(char const *fmt, void (*func)(int, void*), void *arg, int radix, va_list ap);
static void putc_helper(int c, void *arg)
{
	console_putc(c);
}

int
printf(const char *fmt, ...) {
	va_list ap;
	int retval;

	va_start(ap, fmt);
	retval = kvprintf(fmt, putc_helper, 0, 10, ap);
	va_end(ap);

	return (retval);
}
