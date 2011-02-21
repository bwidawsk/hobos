#include "init_funcs.h"

INITFUNCS_CREATE_LIST;

extern void qsort(void *base, uint64_t ndxl, uint64_t ndxr, uint64_t elem_size, int(*compar)(const void *, const void *)); 

#define MAX_INITFUNCS 100
static struct initfuncs *funcs[MAX_INITFUNCS] _INITSECTION_;

int
compare_initfuncs(const void *a, const void *b) {
	struct initfuncs *firstf = *(struct initfuncs **)a;
	struct initfuncs *secondf = *(struct initfuncs **)b;
	uint64_t first = firstf->init_order;
	uint64_t second = secondf->init_order;

	if (first < second)
		return -1;
	if (first == second)
		return 0;
	if (first > second)
		return 1;

	return 0;
}

void
call_initialization_functions() {
	struct initfuncs *funcstruct;
	int index;

	int howmany = INITFUNCS_GETCOUNT();
	KASSERT(howmany < MAX_INITFUNCS, (""));

	INITFUNCS_FOREACH(funcstruct, index) {
		funcs[index] = funcstruct;
	}

	/* 
	 * Sort our init functions 
	 * TODO: ? we use quicksort, which could use a lot of stack, but since we assert
	 * on total elements, it is relatively safe. Also since we only use AMD64 ABI
	 * for now, much less stack is used than with i386 function calling ABI
	 */
	qsort((void *)funcs, 0, (uint64_t)(howmany - 1), sizeof(struct initfunc *), compare_initfuncs);

	for(index = 0; index < howmany; index++) {
		funcs[index]->func_2call_atinit();
	}
}
