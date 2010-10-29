#ifndef __MALLOC_H__
#define __MALLOC_H__

void init_malloc(struct mm_page_allocator *allocator);
void *simple_malloc(uint64_t size);

#define malloc simple_malloc

#endif