#ifndef _COMMON_H_
#define _COMMON_H_

/* TODO: we should find a better way for this */
#ifndef NULL
	#define NULL ((void *)0)
#endif

#define panic(format, ...) do { \
	printf("panic: "); \
	printf(format, ##__VA_ARGS__); \
	__asm__ volatile("ud2"); \
} while (0)

/*
 * The contents should really only contain macros since it will be included
 * by all files.
 */
#ifndef NO_INVARIANTS
#define KASSERT(x, format, ...) do { \
		if (!(x)) { \
			printf("%s: %s\n", __FILE__, #x); \
			backtrace_now(); \
			panic(format, ##__VA_ARGS__); \
		} \
	} while (0)

#define KWARN(x, format, ...) ({ \
	int __warned = !!(x); \
	if (__warned) { \
		printf("Kernel Warning. %s:%d ",__FILE__, __LINE__); \
		printf(format, ##__VA_ARGS__); \
		backtrace_now(); \
	} \
	__warned; \
})

#define KWARN_NOW(x) KWARN(x, "")
#else
	#define KASSERT(x, format, ...)
	#define KWARN(x, ...)
#endif

#define _INITSECTION_ __attribute__((used, section(".recycle")))

/* CompileTimeLIST
 * I got the idea of the implementation from FreeBSD, but it's a generic OS trick
 * thing used in Linux as well. The code is mine. I needed a hint with the __start
 * and __stop to find the start and end points of the section which it seems are
 * automatically made by the linker??? (this is the part I borrowed from FreeBSD) It
 * works by storing a pointer to a structure in a specially named section, we can
 * then later use the special linker symbols to find the start and end of the
 * section and walk through the list.
 */

 /* these macros shouldn't really be publicly used */
#define CTLIST_FIRST_SYM(name) __start_ctlist_##name
#define CTLIST_LAST_SYM(name) __stop_ctlist_##name
#define CTLIST_SECTION(list_name) __attribute__((used, section("ctlist_" #list_name)))

// TODO: size of void star only works if the list is pointers, should be sizeof type
#define CTLIST_COUNT(name) (((void *)&CTLIST_LAST_SYM(name) - (void *)&CTLIST_FIRST_SYM(name)) / sizeof(void *))

/* The following should be publicly used */
/* 
 * Creates a compile type list whose name/key is the first arg, and second arg
 * is the type.
 */
#define CTLIST_CREATE(list_name, type) \
	extern type CTLIST_FIRST_SYM(list_name); \
	extern type CTLIST_LAST_SYM(list_name)
/*
 * Adds an element to list "list_name" with name, type and data
 */
#define CTLIST_ELEM_ADD(list_name, name, type, data) \
	static type name CTLIST_SECTION(list_name) = (type)data;

#define CTLIST_FOREACH(elem, list_name, temp_var) \
	for ((temp_var) = 0, ((elem) = CTLIST_FIRST_SYM(list_name)); temp_var < CTLIST_COUNT(list_name); (temp_var)++, (elem) = *(&CTLIST_FIRST_SYM(list_name) + temp_var))


#define DUMP_BYTES(buf, count) \
	{ \
	int i; \
	for (i = 0; i < count; i++) { \
		if (i && (i % 16 == 0)) \
				printf("\n"); \
		printf("%02x ", ((uint8_t *)buf)[i]); \
	} \
	printf("\n"); \
	}

#ifndef ASM_FILE

/* This should also be somewhere in common/include */
extern int printf(const char *fmt, ...);

typedef int int8_t __attribute__ ((mode (QI)));
typedef unsigned int uint8_t __attribute__ ((mode (QI)));
typedef int int16_t __attribute__ ((mode (HI)));
typedef unsigned int uint16_t __attribute__ ((mode (HI)));
typedef int int32_t __attribute__ ((mode (SI)));
typedef unsigned int uint32_t __attribute__ ((mode (SI)));
typedef int int64_t __attribute__ ((mode (DI)));
typedef unsigned int uint64_t __attribute__ ((mode (DI)));

#include "noarch.h"

#endif

#endif
