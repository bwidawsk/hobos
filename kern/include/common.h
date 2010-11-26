#ifndef _COMMON_H_
#define _COMMON_H_

/* TODO: we should find a better way for this */
#ifndef NULL
	#define NULL ((void *)0)
#endif

/* 
 * The contents should really only contain macros since it will be included
 * by all files.
 */

#ifndef NO_INVARIANTS
#define KASSERT(x, ...) do { \
		if (!(x)) { \
			printf("%s: %s\n", __FILE__, #x); \
			__asm__ volatile("ud2"); \
		} \
	} while (0);
	#define panic(str) KASSERT(0, (str));
#define KWARN(x, ...) do { \
		if (!(x)) { \
			printf("%s: %s\n", __FILE__, #x);  \
		} \
	} while (0);
#else
	#define KASSERT(x, ...)
	#define KWARN(x, ...)
#endif



#define ROUND_UP(x, base) 		(((x) / (base)) * (base)) + (((x) % (base)) ? (base) : 0)
#define ROUND_DOWN(x, base) 	(((x) / (base)) * (base)) 
#define IS_POW2(x) (!(x & (x-1)))
#define INITSECTION  __attribute__ ((section ("initonly") ))
/* CompileTimeLIST
 * I borrowed part of this implementation from FreeBSD, but it's a generic OS 
 * trick thing used in Linux as well.
 * The code is mine. I needed a hint with the __start and __stop to find the 
 * start and end points of the section which it seems are automatically made by 
 * the linker??? (this is the part I borrowed from FreeBSD)
 * It works by storing a pointer to a structure in a specially named section, we
 * can then later use the special linker symbols to find the start and end
 * of the section and walk through the list.
 * usage 
 */

 /* these macros shouldn't really be publicly used */
#define CTLIST_FIRST_SYM(name) __start_ctlist_##name
#define CTLIST_LAST_SYM(name) __stop_ctlist_##name
#define CTLIST_SECTION(list_name) __attribute__((used, section("ctlist_" #list_name)))

#define CTLIST_COUNT(name) (((void *)&CTLIST_LAST_SYM(name) - (void *)&CTLIST_FIRST_SYM(name)) / sizeof(void *))

/* The following should be publicly used */
/* 
 * Creats a compile type list whose name/key is the first arg, and second arg
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


#ifndef ASM_FILE
extern int printf(const char *fmt, ...);
extern volatile unsigned long ticks;
#endif

#endif
