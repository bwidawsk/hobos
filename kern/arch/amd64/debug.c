#include <amd64_defines.h>
#include <ia_defines.h>
#include <mm_amd64.h>

void bt_fp(void *fp)
{
	printf("Backtrace: \n");
	do {
		uint64_t prev_rbp = *((uint64_t *)fp);
		uint64_t prev_ip = *((uint64_t *)(prev_rbp + sizeof(prev_rbp)));
		printf("\t%p\n", prev_ip);
		fp = (void *)prev_rbp;
		/*  Stop if rbp is not in the kernel
		 *  TODO< need an upper bound too*/
		if (fp <= (void *)KVADDR(DMAP_PML,0,0,0))
			break;
	} while(1);
}

void bt()
{
	uint64_t *rbp;

	__asm__ volatile(
					 "movq %%rbp, %0"
					 : "=r" (rbp)
					 :
					 :
					);

	bt_fp(rbp);
}
