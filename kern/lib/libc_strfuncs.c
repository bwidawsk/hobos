#include <stdint.h>

char *__hbuiltin_strcpy_safe(char *dest, const char *src);
char *__hbuiltin_strncpy(char *dest, const char *src, uint64_t n);
int __hbuiltin_strcmp(const char *s1, const char *s2);
int __hbuiltin_strncmp(const char *s1, const char *s2, uint64_t n);
uint64_t __hbuiltin_strlen(const char *s);
	   
char *strcpy(char *dest, const char *src) __attribute__((weak, alias ("__hbuiltin_strcpy_safe")));
char *strncpy(char *dest, const char *src, uint64_t n) __attribute__((weak, alias ("__hbuiltin_strncpy")));
int strcmp(const char *s1, const char *s2) __attribute__((weak, alias ("__hbuiltin_strcmp")));
int strncmp(const char *s1, const char *s2, uint64_t n) __attribute__((weak, alias ("__hbuiltin_strncmp")));
uint64_t strlen(const char *s) __attribute__((weak, alias ("__hbuiltin_strlen")));

// non-compliant strtol
#define STRTOL_DEFAULT_BASE 10
uint64_t strtol(const char *s, int base);

char *
__hbuiltin_strcpy_safe(char *dest, const char *src) {
	#define STRCPY_MAX (1 << 20)
	uint64_t len = strlen(src);
	KASSERT(len < STRCPY_MAX, ());
	return strncpy(dest, src, len);
	
}
/* This was copied from the strncpy man page, modified to use uint64_t */
char*
__hbuiltin_strncpy(char *dest, const char *src, uint64_t n) {
	uint64_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
		dest[i] = src[i];
	for ( ; i < n ; i++)
		dest[i] = '\0';

	return dest;
}

/* Not quite the same as regular strcmp. 0 if equals, !0 otherwise */
int 
__hbuiltin_strcmp(const char *s1, const char *s2) {
	uint64_t len = strlen(s1);
	uint64_t len2 = strlen(s2);
	if (len != len2) {
		return -1;
	}
	
	return strncmp(s1, s2, len);
}

/* Not quite the same as regular strcmp. 0 if equals, !0 otherwise 
 * This was my "test suite"
int main(int argc, char *argv[]) {
    printf("%d\n", __hbuiltin_strncmp("abc", "abc", 3));
    printf("%d\n", __hbuiltin_strncmp("abc", "abc", 2));
    printf("%d\n", __hbuiltin_strncmp("abc", "abc", 4));
    printf("%d\n", __hbuiltin_strncmp("ab", "abc", 4));
    printf("%d\n", __hbuiltin_strncmp("ab", "a", 4));
    return 0;
}

 */
int 
__hbuiltin_strncmp(const char *s1, const char *s2, uint64_t n) {
	while(*s1 != 0 && n > 0) {
		if (*s2 == 0)
			return 1;
			
		if (*s1 < *s2)
			return -1;
			
		if (*s2 < *s1)
			return 1;
		s1++;
		s2++;
		n--;
	}
	
	if (*s1 == 0 && *s2 != 0)
		return -1;

	return 0;
}

uint64_t 
__hbuiltin_strlen(const char *s) {
	uint64_t count = 0;
	while(*s != 0) {
		s++;
		count++;
	}
	return count;
}


static int
is_hex(const char *s) {
	if(s[0] == '0' && (s[1] == 'x'|| s[1] == 'X'))
		return 1;
	return 0;
}

/* Todo check for 9-F */
static int
is_octal(const char *s) {
	if(s[0] == '0' && (!(s[1] == 'x'|| s[1] == 'X')))
		return 1;

	return 0;
}

static int
hex_to_dec(char h) {

	if (h <= '9' && h >= '0')
		return h - '0';

	if (h >= 'a' && h <= 'f')
		return (h - 'a') + 10;

	if (h >= 'A' && h <= 'F')
		return (h - 'A') + 10;

	return 0;
}

uint64_t
strtol(const char *s, int base) {
	uint64_t ret = 0;
	uint64_t temp_base = 1;

#ifdef EXTRA_CAUTIOUS
	if (s == NULL)
		return 0;
#endif
	int length = strlen(s);
	if (base == 0) {
		if (length >= 2) {
			if(is_hex(s)) 
				base = 16;
			if(is_octal(s))
				base = 8;
		} else if(length == 1) {
			if(is_octal(s))
				base = 8;
		} else 
				base = STRTOL_DEFAULT_BASE;
	}

	if (base <= 10) {
		while(length--) {
			ret += (s[length] - '0') * temp_base;
			temp_base *= base;
		}
	} else if (base == 16) {
		while(length--) {
			if(s[length] == 'x' || s[length] == 'X')
				break;
			ret += (hex_to_dec(s[length])) * temp_base;
			temp_base *= base;
		}
	} else {
		ret = 0;
	}

	return ret;
}
