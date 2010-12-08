
/*
 * This was coded by me, but based off of the wikipedia article
 * http://en.wikipedia.org/wiki/Quicksort
 */


/* Important note: use as little stack as possible in all functions, 
 * that's why variables are declared with "register", and we preallocate temp_buf
 * TODO: force inline the whole thing?
 */

/*
 * Simple macro which assumes "base" and "elem_size" exists - calculates
 * the offset of the elements specificed by index ndx.
 */
#define QSORT_TO_ELEM(ndx) (void *)((void *)base + (ndx * elem_size))

#define MAX_ELEM_SIZE 100
uint8_t temp_buf[MAX_ELEM_SIZE];

static void
qswap(void *base, int64_t ndx_a, int64_t ndx_b, uint64_t elem_size) {
	if (ndx_a == ndx_b)
		return;

	// Move a into temp, b into a, and temp into b
	memcpy(temp_buf, QSORT_TO_ELEM(ndx_a), elem_size);
    memcpy(QSORT_TO_ELEM(ndx_a), QSORT_TO_ELEM(ndx_b), elem_size);
    memcpy(QSORT_TO_ELEM(ndx_b), temp_buf, elem_size);
}

static int64_t
partition(void *base, int64_t ndxl, int64_t ndxr, uint64_t elem_size, int(*compar)(const void *, const void *), int64_t pivot_ndx) {
    register uint64_t i;
    register uint64_t store_ndx = ndxl;

    qswap(base, pivot_ndx, ndxr, elem_size);
	void *pivot_elem = QSORT_TO_ELEM(ndxr);

    for (i = ndxl; i < ndxr; i++) {
        if (compar(QSORT_TO_ELEM(i), pivot_elem) < 0) {
            qswap(base, i, store_ndx, elem_size);
            store_ndx++;
        }
    }
    qswap(base, store_ndx, ndxr, elem_size);
    return store_ndx;
}

void
qsort(void *base, int64_t ndxl, int64_t ndxr, uint64_t elem_size, int(*compar)(const void *, const void *)) {
	KASSERT(elem_size < MAX_ELEM_SIZE, ("Unsupported element size for sort\n"));

	if (ndxl >= ndxr)
        return;

    register int64_t pivot_ndx = ndxl + (ndxr - ndxl) / 2;
	KASSERT(pivot_ndx > 0, ());
    register int64_t new_ndx = partition(base, ndxl, ndxr, elem_size, compar, pivot_ndx);
    qsort(base, ndxl, new_ndx - 1, elem_size, compar);
    qsort(base, new_ndx + 1, ndxr, elem_size, compar);
}

