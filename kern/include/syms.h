#ifndef _SYMS_H_
#define _SYMS_H_

struct symbol {
	void *start;
	void *end;
	const char *name;
};

void get_elf_symbols_mboot(multiboot_elf_section_header_table_t *mboot_elf);

#endif
