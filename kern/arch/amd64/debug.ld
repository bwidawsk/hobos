OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
SECTIONS
{
	OS_LOAD_PHYS = 16M;
	. = OS_LOAD_PHYS;
	.multiboot : { *(.multiboot) }
	. = -2048M + OS_LOAD_PHYS + SIZEOF(.multiboot);
	va_base = -2048M;
	kernel_load_start = .;
	load_addr = . + 2048M;
	.text . : AT(load_addr) { *(.text) }
	load_addr = . + 2048M;
	/* There is a bug if I put bss last. It takes all the user defined
	 * sections used by compile time lists, and places it after. It
	 * then expects the loader to do the correct thing with VAs, not PAs
	 * which my loader can't handle. I don't feel this is the end of the
	 * problem, but it works for now.
	 */
	.bss . : AT ( load_addr) 
	{ 
	__bss_start = ABSOLUTE(.);
	*(.bss)
	__bss_end = ABSOLUTE(.);
	}
	load_addr = . + 2048M;
	.data . : AT ( load_addr ) { *(.data)  *(.rodata) }
	kernel_load_end = .;
}
