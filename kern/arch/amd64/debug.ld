OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
SECTIONS
{
	OS_LOAD_PHYS = 16M;
	va_base = -2048M; /* Used for VA to PA conversion */

	.multiboot OS_LOAD_PHYS : AT(OS_LOAD_PHYS) { *(.multiboot) }
	. += va_base;

	__ld_kernel_physical = LOADADDR(.multiboot) + SIZEOF(.multiboot);
	.text ALIGN(0x1000) : AT(__ld_kernel_physical) {
		kernel_load_start = ABSOLUTE(.);
		*(.text);
	}

	__ld_bss_physical = LOADADDR(.text) + SIZEOF(.text);
	.bss ALIGN(0x1000) : AT (__ld_bss_physical) {
		__bss_start = ABSOLUTE(.);
		*(.bss)
		__bss_end = ABSOLUTE(.);
	}

	__ld_data_physical = LOADADDR(.bss) + SIZEOF(.bss);
	.data ALIGN(0x1000) : AT (__ld_data_physical) {
		*(.data) *(.rodata)
		kernel_load_end = ABSOLUTE(.);
	}
}
