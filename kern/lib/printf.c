#include <stdarg.h>
#include <console.h>

int
printf(const char *fmt, ...) {
	va_list ap;
	int retval;

	va_start(ap, fmt);
	retval = kvprintf(fmt, console_putc, 0, 10, ap);
	va_end(ap);

	return (retval);
}
