#ifndef _SYMS_H_
#define _SYMS_H_

struct symbol {
	void *start;
	void *end;
	const char *name;
};

struct sym_offset {
	const char *name;
	uint64_t offset;
};

struct multiboot_elf_section_header_table;
void get_elf_symbols_mboot(struct multiboot_elf_section_header_table *mboot_elf);
struct sym_offset get_symbol(void *addr);

#endif
