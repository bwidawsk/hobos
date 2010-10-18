#ifndef NO_INVARIANTS
#define ASSERT(x) do { \
		if (!(x)) { \
			printf("%s: %s\n", __FILE__, #x); \
			while(1); \
		} \
	} while (0);
#else
#define ASSERT(x)
#endif

void outb(char byte, short port);
void putc(char c);
void puts(const char *str);
void *memset(void *s, int c, int n);
void *memcpy(void *dest, const void *src, int n);
int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
char * strnchr(const char *s, int c, int n);
char * strchr(const char *s, int c);
int printf(const char *format, ...);
