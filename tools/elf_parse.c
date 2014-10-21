#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

void *addr;

#define __ELF_WORD_SIZE 64
#include "../common/include/cdefs.h"
#include "../common/include/elf.h"

static bool debug = true;
#define DEBUG_PRINT(...) do { \
	if (debug) { printf(__VA_ARGS__); } \
} while (0)

void dump_shdr(__ElfN(Shdr) *section)
{
	printf("\tsh_name: %x\n"
		   "\tsh_type: %x\n"
		   "\tsh_flags: %lx\n"
		   "\tsh_addr: %lx\n"
		   "\tsh_offset: %lx\n"
		   "\tsh_size: %lx\n"
		   "\tsh_link: %x\n"
		   "\tsh_info: %x\n"
		   "\tsh_addralign: %lx\n"
		   "\tsh_entsize: %lx\n",
		   section->sh_name,
		   section->sh_type,
		   section->sh_flags,
		   section->sh_addr,
		   section->sh_offset,
		   section->sh_size,
		   section->sh_link,
		   section->sh_info,
		   section->sh_addralign,
		   section->sh_entsize);
}

/* This function is meant to parse a section with the same amount of information
 * that would be contained within the multiboot info.
 */
static void read_sections(uint64_t offset, /* beginning address of ELF */
						  __ElfN(Shdr) *shdr, int shnum, int shstrndx/*, int shentsize */)
{
	__ElfN(Shdr) *sh_string = &shdr[shstrndx];
	char *strings = NULL;
	int i;

	assert(sh_string->sh_type == SHT_STRTAB);

	for (i = 0; i < shnum; i++) {
		if (i == shstrndx)
			continue;

		__ElfN(Shdr) *tmp = &shdr[i];
		if (tmp->sh_type != SHT_STRTAB)
			continue;

		strings = (char *)(offset + tmp->sh_offset);
	}
	for (i = 0; i < shnum; i++) {
		 __ElfN(Shdr) *tmp = &shdr[i];
		 if (tmp->sh_type != SHT_SYMTAB)
			 continue;

		__ElfN(Sym) *symbol_data = (__ElfN(Sym) *)(offset + tmp->sh_offset);
		int j;
		for (j = 0; j <  tmp->sh_size / sizeof(__ElfN(Sym)); j++) {
			__ElfN(Sym) *symbol = &symbol_data[j];
			if (ELF64_ST_TYPE(symbol->st_info) == STT_FUNC)
				DEBUG_PRINT("%s %lx %lx\n",
							&strings[symbol->st_name], symbol->st_value, symbol->st_size);
		}
	}
}

static void
verify_elf(__ElfN(Ehdr) *elf_header)
{
	const char elf_magic[4] = {0x7f, 'E', 'L', 'F'};
	uint8_t *e_ident = (uint8_t *)elf_header;
	assert(strncmp((const char *)elf_header, elf_magic, 4) == 0);
	assert(e_ident[EI_CLASS] == ELFCLASS64);
	/* TODO: more verification */
}


int main(int argc, char *argv[])
{
	int fd;
	struct stat sb;
	__ElfN(Half) shnum, shstrndx;

	fd = open(argv[1], O_RDONLY);
	assert(fd > 0);
	assert(fstat(fd, &sb) != -1);

	DEBUG_PRINT("Opened file %s\n", argv[1]);

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(addr != MAP_FAILED);

	verify_elf(addr);

	__ElfN(Ehdr) *ehdr = addr;
	assert(ehdr->e_shentsize == sizeof(__ElfN(Shdr)));

	shnum = ehdr->e_shnum;
	DEBUG_PRINT("There are %d sections\n", shnum);
	shstrndx = ehdr->e_shstrndx;

	if (shstrndx == SHN_UNDEF) {
		DEBUG_PRINT("No section strings :-(\n");
	} else {
		DEBUG_PRINT("Index %d contains the section strings\n", shstrndx);
	}

	__ElfN(Shdr) *shdr = (__ElfN(Shdr) *)(addr + ehdr->e_shoff);
	read_sections((uint64_t)addr, shdr, shnum, shstrndx);

	munmap(addr, sb.st_size);
	return 0;
}
