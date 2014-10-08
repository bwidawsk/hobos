#ifndef va_list
#include <stdarg.h>
#endif

#include "small_libc.h"

/* TODO: outb is needed by putc, and this putc is bochs specific, remove it */
void
outb(char byte, short port) {
	__asm__ volatile("out %0, %1" : : "a" (byte), "d" (port));
}

/*
 * It's easiest to just use the Bochs debug port 0xe9 for now
 */
void
putc(char c) {
	outb(c, 0xe9);
}

void
puts(const char *str) {
	while(*str != 0) {
		putc(*str);
		str++;
	}
}

void *
memset(void *s, int c, int n) {
	while(n--)
		*(unsigned char *)s++ = (unsigned char)c;

	return s;
}

void *
memcpy(void *dest, const void *src, int n) {
	int dir = 0;
	if ((src < dest) && ((src + n) > dest))
		dir = 1;

	if (dir) {
		dest = dest + n - 1;
		src = src + n - 1;
		while(n--)
			*(unsigned char *)dest-- = *(unsigned char *)src--;
	} else {
		while(n--)
			*(unsigned char *)dest++ = *(unsigned char *)src++;
	}

	return dest;
}

int
strlen(const char *s) {
	int count = 0;
	if (s == 0)
		return 0;

	while(*s != 0) {
		count++;
		s++;
	}
	return count;
}

int
strcmp(const char *s1, const char *s2) {
	int matched = 0;
	if(s1 == 0 || s2 == 0)
		return 1;

	while(*s1 != 0) {
		if(*s2 == 0 || *s2 != *s1) {
			return 1;
		}
		matched++;
		s1++;
		s2++;
	}

	if (*s1 != *s2) {
		return 1;
	}

	return !matched;
}

int
strncmp(const char *s1, const char *s2, int n) {
	int matched = 0;
	if(s1 == 0 || s2 == 0)
		return 1;

	while(*s1 != 0 && n--) {
		if(*s2 == 0 || *s2 != *s1) {
			return 1;
		}
		matched++;
		s1++;
		s2++;
	}

	if (n > -1 && (*s1 != *s2)) {
		return 1;
	}

	return !matched;
}
char *
strnchr(const char *s, int c, int n)
{
	if (n == -1)
		n = (0x7FFFFFFF);

	while(*s != 0 && n--) {
		if (*s == (char)c) {
			return (char *)s;
		}

		s++;
	}
	return 0;
}

char *
strchr(const char *s, int c) {
	return strnchr(s, c, -1);
}

static inline void
num_to_hex(unsigned int num, char ret[8]) {
	char hex_chars[] = "0123456789abcdef";
	int i = 8;
	while(i--)
		ret[7-i] = hex_chars[(num >> (i*4))& 0xf];
}

#define PRINTF_BUF_MAX 512
int
printf(const char *format, ...) {
	char test[PRINTF_BUF_MAX];
	char num[8];
	char *instr;
	unsigned int innum;
	int strsize;
	int count = 0;
	const char *string = format;
	va_list ap;

	if (format == (const char *)0)
		return 0;

	if (format[0] == (char)0)
		return 0;

	if (format[1] == (char)0) {
		putc(format[0]);
		return 1;
	}

	memset(test, 0, PRINTF_BUF_MAX);
	va_start(ap, format);

	do {
		if (*string == '%') {
			switch(string[1]) {
				case 'x':
					innum = va_arg(ap, unsigned int);
					num_to_hex(innum, num);
					memcpy(&test[count], num, 8);
					count += 8;
					break;

				case 's':
					instr = va_arg(ap, char*);
					strsize = strlen(instr);
					memcpy(&test[count], instr, strsize);
					count += strsize;
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
		if (count >= 255) {
			break;
		}
	} while (*string != 0);

	va_end(ap);
	puts(test);
	return 0;
}

