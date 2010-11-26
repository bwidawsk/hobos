#ifndef __MALLOC_H__
#define __MALLOC_H__
#include "page.h"
#include "page_allocator.h"

void init_malloc(struct mm_page_allocator *allocator);
void *simple_malloc(uint64_t size);
void simple_free(void *addr);

#define malloc simple_malloc
#define free simple_free

#endif
