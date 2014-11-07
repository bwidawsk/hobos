#include <amd64_defines.h>
#include <ia_defines.h>
#include <mm_amd64.h>
#include <syms.h>

void backtrace(void *fp)
{
	do {
		uint64_t prev_rbp = *((uint64_t *)fp);
		uint64_t prev_ip = *((uint64_t *)(fp + sizeof(prev_rbp)));
		struct sym_offset sym_offset = get_symbol((void *)prev_ip);
		printf("\t%s (+0x%x)\n", sym_offset.name, sym_offset.offset);
		fp = (void *)prev_rbp;
		/*  Stop if rbp is not in the kernel
		 *  TODO< need an upper bound too*/
		if (fp <= (void *)KVADDR(DMAP_PML,0,0,0))
			break;
	} while(1);
}

void backtrace_now(void)
{
	uint64_t rbp;
	uint64_t ip;

	__asm__ volatile("call whatevs%=;"
					 "whatevs%=:pop %1;"
					 "movq %%rbp, %0;"
					 : "=r" (rbp), "=r" (ip)
					 :
					 :
					);
	struct sym_offset sym_offset = get_symbol((void *)ip);
	printf("\t%s (+0x%x)\n", sym_offset.name, sym_offset.offset);
	backtrace((void *)rbp);
}
