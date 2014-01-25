#ifndef __MALLOC_H__
#define __MALLOC_H__
#include "page.h"
#include "page_allocator.h"

/* Malloc used before initfuncs. Try to remove this eventually */
void init_early_malloc(struct mm_page_allocator *allocator);

void *simple_malloc(uint64_t size);
void simple_free(const void *addr);

#define malloc simple_malloc
#define free simple_free

#endif
