#define	__CAT1(x,y)	x ## y
#define	__CAT(x,y)	__CAT1(x,y)

#define OFFSET_OF(structure, member) __builtin_offsetof(structure, member)
#define CONTAINER_OF(embedded_ptr, container_type, member_name) \
	(container_type *)(((void *)(embedded_ptr)) - OFFSET_OF(container_type, member_name))
