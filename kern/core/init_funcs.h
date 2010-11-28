#define INITFUNCS_KEY sect_initfuncs

#define INITFUNCS_CREATE_LIST CTLIST_CREATE(INITFUNCS_KEY, struct initfuncs *);

#define INITFUNCS_DECLARE(initfuncs_name, func, order) \
    static struct initfuncs initfuncs_name = { \
        .name = #initfuncs_name, \
        .func_2call_atinit = (init_function_t)func, \
		.init_order = order \
        }; \
    CTLIST_ELEM_ADD(INITFUNCS_KEY, initfuncs_name##_list_ptr, struct initfuncs *, (struct initfuncs *)&initfuncs_name);

/* Gives a pointer to each initfuncs device. */
#define INITFUNCS_FOREACH(elem, garbage) \
    CTLIST_FOREACH(elem, INITFUNCS_KEY, garbage)

#define INITFUNCS_GETCOUNT() CTLIST_COUNT(INITFUNCS_KEY)

#define INITFUNC_DECLARE(name) static void name()
typedef void (*init_function_t)();

struct initfuncs {
	char *name;
	init_function_t func_2call_atinit;
	uint64_t init_order;
};
