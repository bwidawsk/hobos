/* move this elsewhere at some point */
#define __ELF_WORD_SIZE 64
#include "small_libc.h"
#include "multiboot.h"


#include "stdint.h"
#include "cdefs.h"
#include "elf.h"

#define ELF_PRINTF(...) printf(__VA_ARGS__)

static __ElfN(Ehdr) *
validate_header64(const void *addr) {
	const char elf_magic[4] = { 0x7f, 'E', 'L', 'F' };
	char *e_ident = (char *)addr;
	int ret = strncmp(e_ident, elf_magic, 4);
	//if (!IS_ELF(ehdr))
	if (ret) {
		ELF_PRINTF("not an elf\n");
		return 0;
	}

	if (e_ident[EI_CLASS] != ELF_CLASS) {
		ELF_PRINTF("not the correct elf loader for class\n");
		return 0;
	}
	if (e_ident[EI_DATA] != ELFDATA2LSB) {
		ELF_PRINTF("currently only able to handle LSB ELFs\n");
		return 0;
	}

	__ElfN(Ehdr) *ehdr = (__ElfN(Ehdr) *) addr;
	if (ehdr->e_type != ET_EXEC) {
		ELF_PRINTF("Can only load executable files\n");
		return 0;
	}

	/* TODO: this shouldn't be hardcoded here */
	if (ehdr->e_machine != EM_AMD64) {
		ELF_PRINTF("Can only load 386 machines\n");
		return 0;
	}

	return ehdr;
}

#define PADDR_TO_PTR(foo) ((void *)((uint32_t)foo))

/* Returns address of where section header info */
__ElfN(Addr)
elf_load64(const void *addr, unsigned int loaded_len,
	struct multiboot_elf_section_header_table *ret) {
	__ElfN(Ehdr) *ehdr = validate_header64(addr);
	if (ehdr == 0)
		return 0;

	__ElfN(Phdr) *phdr = (__ElfN(Phdr) *)(addr + ehdr->e_phoff);
	ASSERT(phdr != 0);

	ASSERT(ehdr->e_phentsize == sizeof(__ElfN(Phdr)));
	__ElfN(Half) phdr_count = ehdr->e_phnum;

	while(phdr_count--) {
		if (phdr[phdr_count].p_type == PT_LOAD) {
			/* TODO: we could relocate things also, it's easy for 1 segment,
			 * but a pain for more than 1
			 */

			ASSERT(phdr[phdr_count].p_memsz >= phdr[phdr_count].p_filesz);
			ASSERT ((uint32_t)addr > (phdr[phdr_count].p_paddr + phdr[phdr_count].p_memsz) ||
				(((uint32_t)addr + loaded_len) < phdr[phdr_count].p_paddr));

			/* This assertion is platform specific and should be removed */
			ASSERT(phdr[phdr_count].p_paddr > 0x100000);
			memcpy(PADDR_TO_PTR(phdr[phdr_count].p_paddr),
				(phdr[phdr_count].p_offset + addr),
				(phdr[phdr_count].p_filesz));
			memset(PADDR_TO_PTR(phdr[phdr_count].p_paddr + (uint32_t)phdr[phdr_count].p_filesz),
				0,
				phdr[phdr_count].p_memsz - phdr[phdr_count].p_filesz);
		} 
	}

	ret->num = (multiboot_uint32_t)ehdr->e_shnum;
	ret->size = (multiboot_uint32_t)ehdr->e_shentsize;
	ret->addr = (multiboot_uint32_t)ehdr->e_shoff + (multiboot_uint32_t)addr;
	ret->shndx = (multiboot_uint32_t)ehdr->e_shstrndx;

	return ehdr->e_entry;
}
