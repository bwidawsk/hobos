#define	__CAT1(x,y)	x ## y
#define	__CAT(x,y)	__CAT1(x,y)

#define OFFSET_OF(structure, member) __builtin_offsetof(structure, member)
#define CONTAINER_OF(embedded_ptr, container_type, member_name) \
	(container_type *)(((void *)(embedded_ptr)) - OFFSET_OF(container_type, member_name))

#define ROUND_UP(x, base)		((((x) / (base)) * (base)) + (((x) % (base)) ? (base) : 0))
#define ROUND_DOWN(x, base)		(((x) / (base)) * (base))
#define DIV_ROUND_UP(x, y)				(x) % (y) ? (x) / (y) + 1 : (x) / (y)
#define IS_POW2(x) (!(x & (x-1)))
