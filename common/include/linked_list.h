#ifndef _LINKED_LIST_
#define _LINKED_LIST_

#ifdef KERNEL
 #define LINK_WARN(x) KWARN_NOW(x)
 #define LINK_ASSERT(...) KASSERT(0, ##__VA_ARGS__)
#else
 #define LINK_WARN(x) fprintf(stderr, x)
 #define LINK_ASSERT(x)	assert(x)
#endif

struct linked_list {
	struct linked_list *next;
};

#define SLIST_DECLARE(name)			\
	struct linked_list name;		\
	name.next = NULL

#define SLIST_INIT(elem) do {		\
	(elem)->next = NULL;			\
} while (0)

#define SLIST_EMPTY(list) ((list)->next == NULL)

#define SLIST_FIRST(list) ((list)->next)
#define SLIST_FIRST_ENTRY(list, type, member) CONTAINER_OF(SLIST_FIRST(list), type, member)
static inline struct linked_list *slist_last(struct linked_list *list)
{
	struct linked_list *iter;
	if (SLIST_EMPTY(list))
		return NULL;

	for (iter = list->next; iter->next != NULL; iter = iter->next);

	return iter;
}

#define SLIST_PREPEND(elem, list) do {		\
	if ((elem)->next != NULL)				\
		LINK_ASSERT("Adding bad element\n");\
	(elem)->next = list->next;				\
	(list)->next = elem;					\
} while (0)

/* TODO: When doubly linked lists are defined, make a WARN here as it should never
 * be used */
#define SLIST_APPEND(elem, list) do {		\
	struct linked_list *__iter;				\
	if ((elem)->next != NULL)				\
		LINK_ASSERT("Adding bad element\n");\
	if (SLIST_EMPTY(list)) {				\
		(list)->next = elem;				\
		break;								\
	}										\
	for (__iter = (list)->next; __iter->next != NULL; __iter = __iter->next);	\
	__iter->next = elem;						\
} while (0)

#define SLIST_ADD(elem, list) SLIST_PREPEND((elem), (list))

#define SLIST_FOR_EACH(iter, list) \
	for (iter = (list)->next; iter != NULL; iter = iter->next)

#define SLIST_FOR_EACH_ENTRY(iter, list, member)							\
	for (iter = CONTAINER_OF((list)->next, typeof(*(iter)), member);			\
		 iter != NULL;														\
		 iter = CONTAINER_OF((iter)->member.next,  typeof(*(iter)), member))

#define SLIST_REMOVE(elem, list) do {		\
	struct linked_list *__elem = elem;	 /* Is this needed to evaluate a macro once? */ \
	struct linked_list *iter, *prev = list;	\
	LINK_ASSERT(list != NULL);				\
	SLIST_FOR_EACH(iter, list) {			\
		if (__elem == iter) {				\
			LINK_ASSERT(prev->next == __elem); \
			prev->next = __elem->next;		\
			__elem->next = NULL;			\
			break;							\
		}									\
		prev = iter;						\
	}										\
} while (0)

/* TODO: Circular doubly linked lists */
struct doubly_linked_list {
	struct doubly_linked_list *next;
	struct doubly_linked_list *prev;
};

#define DLIST_DECLARE(name)				\
	struct doubly_linked_list name;		\
	name.next = NULL;					\
	name.prev = NULL

#define DLIST_INIT(elem) do {		\
	(elem)->next = NULL;			\
	(elem)->prev = NULL;			\
} while (0)

#define DLIST_EMPTY(list) ((list)->next == NULL)
#endif
