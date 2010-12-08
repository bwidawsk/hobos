#ifdef AMD64
	#include "../arch/ia_common/ia_defines.h"
#endif
#define PAGE_SIZE			(1UL << PAGE_SHIFT)
#define PAGE_MASK			~(PAGE_SIZE - 1UL)
#define PAGE_OFFSET_MASK	(PAGE_SIZE - 1UL)
#define PAGE_OFFSET(x)		((x) & PAGE_OFFSET_MASK)
#define PAGE_FROM_VAL(x)	((x) >> PAGE_SHIFT)
#define PAGE_TO_VAL(x)		((x) << PAGE_SHIFT)

typedef uint64_t pfn_t;

struct mm_page {
	uint64_t pindex;
	uint64_t pa;
};
