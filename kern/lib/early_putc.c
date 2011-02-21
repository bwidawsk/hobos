#include <stdarg.h> // This is defined in common/include

/* Simple ring buffer for holding char data */
char buf[64];
unsigned char ptr=0;

void
__hbuiltin_early_putc(char c) {
	buf[ptr++] = c;
	ptr &= 64;
}

void early_putc(char c) __attribute__((weak, alias ("__hbuiltin_early_putc")));

#if (CONFIG_STUBBED_EARLY_PRINTF == 0)
int
early_printf(const char *format, ...) {
	return 0;
}
#else

void early_puts(char *string) {
	while (*string != 0) {
		early_putc(*string);
		string++;
	}
}
static inline void
num_to_hex(unsigned long long num, char ret[16]) {
	char hex_chars[] = "0123456789abcdef";
	int i = 16;
	while(i--)
		ret[15-i] = hex_chars[(num >> (i*4))& 0xf];
}
#define PRINTF_BUF_MAX 512
static char test[PRINTF_BUF_MAX];
int 
early_printf(const char *format, ...) {
	char num[16] = {0};
	char *instr, *tmpstr;
	unsigned long long innum;
	int strlength;
	int count = 0;
	int i;
	const char *string = format;
	va_list ap;

	if (format == (const char *)0) 
		return 0;

	if (format[0] == (char)0)
		return 0;

	if (format[1] == (char)0) {
		early_putc(format[0]);
		return 1;
	}

	//memset(test, 0, PRINTF_BUF_MAX);
	for ( i = 0; i < PRINTF_BUF_MAX; i++ ) {
		test[i] = 0;
	}
	va_start(ap, format);

	do {
		if (*string == '%') {
			switch(string[1]) {
				case 'x':
					innum = va_arg(ap, unsigned long long);
					num_to_hex(innum, num);
					// 16 number of nybbles
					for( i = 0; i < 16; i++) {
						test[count + i] = num[i];
					}
					count += 16;
					break;

				case 's':
					instr = va_arg(ap, char*);
					if (instr == 0)
						break;

					tmpstr = instr;
					// bw: for some reason it doesn't work without = 0
					strlength = 0;
					while(*tmpstr) { strlength++; tmpstr++;}
					for( i = 0; i < strlength; i++) {
						test[count + i] = instr[i];
					}
					count += strlength;
					break;

				default:
					break;
			}
			string++;
		} else {
			test[count] = *string;
			count++;
		}

		string++;
		if (count >= PRINTF_BUF_MAX - 1) {
			break;
		}
	} while (*string != 0);

	va_end(ap);
	test[count] = 0;
	early_puts(test);
	return count;
}
#endif
