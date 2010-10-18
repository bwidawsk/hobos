SECTIONS
{
	. = 0x7c00;

	.boottext : { 
		head.o(*.text)
		bootstrap.o(*.text)
		legacy_int.o(*.text)
		tramp.o(*.text)
	}

	.bootbss : {
		_bstart = .;
		bootstrap.o(.bss)
		_bend = .;
	}
	. = ALIGN(0x200);

	ASSERT(. == 0x7e00, "bootstrap didn't fit in one sector")

	.text : {
		_btext = .;
		smallboot.o(*.text)
		*(*.text)
		_etext = .;
	}

	.data : {
		_data = .;
		*(.data);
		_edata = . ;
	}

	.bss : {
		_realbstart = .;
		*(.bss)
		*(COMMON) ;
		_realbend = .;
	}

	ASSERT(ADDR(.data) + SIZEOF(.data) < 0xF800, "uh oh")
	ASSERT(ADDR(.bss) + SIZEOF(.bss) < 0x9F000, "uh oh")

	/DISCARD/ : {
	}
}
