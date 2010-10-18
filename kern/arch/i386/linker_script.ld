SECTIONS
{
	. = 0x1000000;
	.multiboot : { *(.multiboot) }
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
