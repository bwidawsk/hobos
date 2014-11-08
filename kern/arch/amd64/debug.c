#include <amd64_defines.h>
#include <ia_defines.h>
#include <mm_amd64.h>
#include <include/arch.h>
#include <syms.h>

void backtrace(void *fp)
{
	do {
		uint64_t prev_rbp = *((uint64_t *)fp);
		uint64_t prev_ip = *((uint64_t *)(fp + sizeof(prev_rbp)));
		struct sym_offset sym_offset = get_symbol((void *)prev_ip);
		printf("\t%s (+0x%x) [0x%llx]\n",
			   sym_offset.name, sym_offset.offset, prev_ip);
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

static void
dump_arch_state(struct arch_state *arch)
{
	printf("Registers:\n"
		   "RAX=0x%016llx\tRBX=0x%016llx\n"
		   "RCX=0x%016llx\tRDX=0x%016llx\n"
		   "R8= 0x%016llx\tR9= 0x%016llx\n"
		   "R10=0x%016llx\tR11=0x%016llx\n"
		   "R12=0x%016llx\tR13=0x%016llx\n"
		   "R14=0x%016llx\tR15=0x%016llx\n"
		   "RDI=0x%016llx\tRSI=0x%016llx\n"
		   "RBP=0x%016llx\tRSP=0x%016llx\n",
		   arch->rax, arch->rbx, arch->rcx, arch->rdx,
		   arch->r8, arch->r9, arch->r10, arch->r11,
		   arch->r12, arch->r13, arch->r14, arch->r15,
		   arch->rdi, arch->rsi, arch->rbp, arch->rsp);
}

#include <console.h>
#include <bs_commands.h>
static void *
show_arch_state(struct console_info *info, int argc, char *argv[])
{
	if (!info || !info->arch_state)
		dump_arch_state(get_arch_state(0));
	else
		dump_arch_state(info->arch_state);

	return NULL;
}

static void
arch_state_help(void)
{
	printf("Show the current (or fault) architectural state.\n"
		   "Current state is actually at the time of the console starting.\n");
}

BS_COMMAND_DECLARE(arch, show_arch_state, arch_state_help);
