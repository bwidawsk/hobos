#define INITFUNCS_KEY sect_initfuncs

#define INITFUNCS_CREATE_LIST CTLIST_CREATE(INITFUNCS_KEY, struct initfuncs *);

#define INITFUNCS_DECLARE_OLD(initfuncs_name, func, order) \
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

#define INITFUNC_DECLARE(initfuncs_name, order) \
	static void _INITSECTION_ initfuncs_name(); \
	static struct initfuncs initfuncs_name##INITFUNCS_KEY = { \
        .name = #initfuncs_name, \
        .func_2call_atinit = (init_function_t)initfuncs_name, \
		.init_order = order \
        }; \
    CTLIST_ELEM_ADD(INITFUNCS_KEY, initfuncs_name##_list_ptr, struct initfuncs *, (struct initfuncs *)&initfuncs_name##INITFUNCS_KEY); \
	static void _INITSECTION_ initfuncs_name()

typedef void (*init_function_t)();

#define INITFUNC_SUBSYSTEM_FIRST	0x00
#define INITFUNC_SUBSYSTEM_EARLY	0x01
#define INITFUNC_SUBSYSTEM_ANY		0xff

// All machine specific here
#define INITFUNC_ARCHINIT		0x01000000UL

// Whatever is needed to get page allocators + malloc up
#define INITFUNC_ALLOCATION		0x02000000UL
#define INITFUNC_ALLOCATION_PAGE		(0x02000000UL + INITFUNC_SUBSYSTEM_FIRST)
#define INITFUNC_ALLOCATION_SUBSYSTEM	(0x02000000UL + INITFUNC_SUBSYSTEM_ANY)

// All device specific here
#define INITFUNC_DEVINIT				 0x10000000UL
#define INITFUNC_DEVICE_BLOCK			 0x10002000UL
#define INITFUNC_DEVICE_BLOCK_ANY		(0x10002000UL + INITFUNC_SUBSYSTEM_ANY)
#define INITFUNC_DEVICE_VFS_SUBSYSTEM	(0x10FFFF00UL + INITFUNC_SUBSYSTEM_FIRST)
#define INITFUNC_DEVICE_FS				(INITFUNC_DEVICE_VFS_SUBSYSTEM + \
										 INITFUNC_SUBSYSTEM_EARLY)
#define INITFUNC_DEVICE_VFS				(INITFUNC_DEVICE_VFS_SUBSYSTEM + \
										 INITFUNC_SUBSYSTEM_ANY)

struct initfuncs {
	char *name;
	init_function_t func_2call_atinit;
	uint64_t init_order;
};

void call_initialization_functions();
