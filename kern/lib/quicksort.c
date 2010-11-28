#include <stdint.h>

/*
 * This was coded by me, but based off of the wikipedia article
 * http://en.wikipedia.org/wiki/Quicksort
 */

#define QSORT_TO_ELEM(ndx) (void *)((void *)base + (ndx * elem_size))

static void
qswap(void *base, uint64_t ndx_a, uint64_t ndx_b, uint64_t elem_size) {
    uint8_t temp[elem_size];
    memcpy(temp, base + (ndx_a * elem_size), elem_size);
    memcpy(base + (ndx_a * elem_size),  base + (ndx_b * elem_size), elem_size);
    memcpy(base + (ndx_b * elem_size), temp, elem_size);
}

static uint64_t
partition(void *base, uint64_t ndxl, uint64_t ndxr, uint64_t elem_size, int(*compar)(const void *, const void *), uint64_t pivot_ndx) {
	void *pivot_elem = QSORT_TO_ELEM(pivot_ndx);
    uint64_t i;
    uint64_t store_ndx = ndxl;

    qswap(base, pivot_ndx, ndxr, elem_size);

    for (i = ndxl; i < ndxr; i++) {
        if (compar(QSORT_TO_ELEM(i), pivot_elem) <= 0) {
            qswap(base, i, store_ndx, elem_size);
            store_ndx++;
        }
    }
    qswap(base, store_ndx, ndxr, elem_size);
    return store_ndx;
}

void
qsort(void *base, uint64_t ndxl, uint64_t ndxr, uint64_t elem_size, int(*compar)(const void *, const void *)) {
    uint64_t pivot_ndx = ndxl + (ndxr - ndxl) / 2;
    uint64_t new_ndx = partition(base, ndxl, ndxr, elem_size, compar, pivot_ndx);
    qsort(base, ndxl, new_ndx - 1, elem_size, compar);
    qsort(base, new_ndx + 1, ndxr, elem_size, compar);
}

