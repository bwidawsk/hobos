extern void memcpy(void *dest, const void *src, uint64_t n);
extern void memset(void *s, int c, uint64_t n);
extern void bzero(void *s, uint64_t n);
extern void bcopy(const void *src, void *dest, uint64_t n);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, uint64_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, uint64_t n);
extern uint64_t strlen(const char *s);

