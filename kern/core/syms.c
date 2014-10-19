#include <mm/malloc.h>
#include <multiboot.h>
#define __ELF_WORD_SIZE 64 /* FIXME: need to figure how to abstract */
#include <elf.h>
#include <arch/arch.h> // arch_xlate_pa()
#include <lib.h>

#include "syms.h"

static struct {
	struct symbol *symbols;
	int count;
} symbol_table;

static void
get_elf64_symbols_mboot(uint32_t section_headers, Elf64_Half shnum, Elf64_Half shstrndx)
{
	Elf64_Shdr *sh_table;
	char *strings = NULL;
	Elf64_Sym *syms = NULL;
	int elf_sym_count = 0;
	int function_syms = 0;
	int i;

	sh_table = arch_xlate_pa((void *)(uint64_t)section_headers);

	for (i = 0; i < shnum; i++) {
		if (i == shstrndx)
			continue;

		if (sh_table[i].sh_type == SHT_STRTAB) {
			KASSERT(strings == NULL, "Can't handle multiple string tables\n");

			if (sh_table[i].sh_addr == 0)
				continue;

			strings = malloc(sh_table[i].sh_size);
			if (KWARN(!strings, "Failed to allocate memory for strings\n"))
				return;

			memcpy(strings, arch_xlate_pa((void *)sh_table[i].sh_addr), sh_table[i].sh_size);
		}

		if (sh_table[i].sh_type == SHT_SYMTAB) {
			int j;

			KASSERT(syms == NULL, "Can't handle multiple symbol tables\n");

			if (sh_table[i].sh_addr == 0)
				continue;

			syms = (Elf64_Sym *)arch_xlate_pa((void *)sh_table[i].sh_addr);
			elf_sym_count = sh_table[i].sh_size / sizeof(Elf64_Sym);

			for (j = 0; j < elf_sym_count; j++) {
				Elf64_Sym *sym = &syms[j];
				if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC && sym->st_size && sym->st_name) {
					function_syms++;
				}
			}
		}
	}

	KWARN(!strings, "No string section found in ELF\n");

	if (KWARN(!syms, "No symbol section found in ELF\n"))
		free(strings);

	if (!strings || !syms)
		return;

	symbol_table.symbols = malloc(sizeof(struct symbol) * function_syms);
	if (!symbol_table.symbols) {
		KWARN(1, "Failed to allocate space for symbols\n");
		free(strings);
		return;
	}

	/* Store count for walking the array later */
	symbol_table.count = function_syms;

	for (i = 0; i < elf_sym_count; i++) {
		Elf64_Sym *sym = &syms[i];
		if (ELF64_ST_TYPE(sym->st_info) != STT_FUNC || !sym->st_size)
			continue;

		symbol_table.symbols[--function_syms].start = (void *)sym->st_value;
		symbol_table.symbols[function_syms].end = (void *)sym->st_value + sym->st_size;
		symbol_table.symbols[function_syms].name = &strings[sym->st_name];
	}

	KWARN(function_syms, "Leftover %d symbols, space wasted\n", function_syms);
	/* Adjust the pointer if there are leftovers */
	symbol_table.count -= function_syms;
	symbol_table.symbols = &symbol_table.symbols[function_syms];
}

/* Check if the address exists in the given symbol. Returns the offset from the
 * beginning if so */
static inline uint64_t
__symbol_offset(void *addr, struct symbol *sym)
{
	if (addr >= sym->end)
		return 0;

	if (addr < sym->start)
		return 0;

	return addr - sym->start;
}

struct sym_offset
get_symbol(void *addr)
{
	struct symbol *sym;
	int i;

	for (i = 0; i < symbol_table.count; i++) {
		sym = &symbol_table.symbols[i];
		uint64_t offset = __symbol_offset(addr, sym);
		if (offset)
			return (struct sym_offset){sym->name, offset};
	}

	return (struct sym_offset){"????", 0};
}

void
get_elf_symbols_mboot(multiboot_elf_section_header_table_t *mboot_elf)
{
	get_elf64_symbols_mboot(mboot_elf->addr, mboot_elf->num, mboot_elf->shndx);
}
