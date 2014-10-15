#include <mm/malloc.h>
#include <multiboot.h>
#define __ELF_WORD_SIZE 64 /* FIXME: need to figure how to abstract */
#include <elf.h>
#include <arch/arch.h> // arch_xlate_pa()

static struct symbol {
	uint64_t start;
	uint64_t end;
	const char * name;
} *kernel_symbols;


static void
get_elf64_symbols_mboot(Elf64_Shdr *sh_table, Elf64_Half shnum, Elf64_Half shstrndx)
{
	char *strings = NULL;
	Elf64_Sym *syms = NULL;
	int elf_sym_count = 0;
	int function_syms = 0;
	int i;

	sh_table = arch_xlate_pa(sh_table);

	for (i = 0; i < shnum; i++) {
		if (i == shstrndx)
			continue;

		if (sh_table[i].sh_type == SHT_STRTAB) {
			KASSERT(strings == NULL, "Can't handle multiple string tables\n");

			if (sh_table[i].sh_addr == 0)
				continue;

			strings = malloc(sh_table[i].sh_size);
			if (!strings) {
				KWARN("Failed to allocate memory for strings\n");
				return;
			}
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
				Elf64_Sym *sym = &syms[i];
				if (sym->st_info == STT_FUNC && sym->st_size && sym->st_name) {
					function_syms++;
				}
			}
		}
	}

	if (!strings) {
		KWARN("No string section found in ELF\n");
	}

	if (!syms) {
		KWARN("No symbol section found in ELF\n");
		free(strings);
	}

	if (!strings || !syms)
		return;

	kernel_symbols = malloc(sizeof(struct symbol) * function_syms);
	if (!kernel_symbols) {
		KWARN("Failed to allocate space for symbols\n");
		free(strings);
		return;
	}

	for (i = 0; i < elf_sym_count; i++) {
		Elf64_Sym *sym = &syms[i];
		if (sym->st_info != STT_FUNC || !sym->st_size)
			continue;
		kernel_symbols[--function_syms].start = sym->st_value;
		kernel_symbols[function_syms].end = sym->st_value + sym->st_size;
		kernel_symbols[function_syms].name = &strings[sym->st_name];
	}

	KWARN(function_syms, "Leftover symbols\n");
}

void
get_elf_symbols_mboot(multiboot_elf_section_header_table_t *mboot_elf)
{
	get_elf64_symbols_mboot(mboot_elf->addr, mboot_elf->num, mboot_elf->shndx);
}
