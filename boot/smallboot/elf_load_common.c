#include "small_libc.h"
#include "multiboot.h"
#include "stdint.h"
#include "cdefs.h"
#include "elf.h"

#define ELF_PRINTF(...) printf(__VA_ARGS__)

/* XXX: Something to keep in mind is that even though we have the ElfN macro to do
 * the dirty 32/64 disambiguation for us, the bootloader is always built as 32b, ie.
 * pointers are 32b. */

/* It would waste space to have to define this helper function twice. We can make it
 * word size independent pretty easily. The one hack is get_load_addr() */

#define get_load_addr(phdr) ((void *)(((uint32_t)(phdr)->p_vaddr)&0x7FFFFFFFU))
#ifndef load_segments
static void
load_segments(const void *addr, __ElfN(Phdr) *phdr, int phdr_count)
{
	int i;

	for (i = 0; i < phdr_count; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			unsigned int pad_space;
			void *pad_start;
			/* TODO: we could relocate things also, it's easy for 1 segment,
			 * but a pain for more than 1
			 */
			ASSERT(phdr[i].p_memsz >= phdr[i].p_filesz);

			/* This assertion is platform specific and should be removed */
			ASSERT(phdr[i].p_paddr > 0x100000);

			memcpy(get_load_addr(&phdr[i]),
				((phdr[i].p_offset) + addr),
				(phdr[i].p_filesz));
			pad_space = phdr[i].p_memsz - phdr[i].p_filesz;
			pad_start = get_load_addr(&phdr[i]) + phdr[i].p_filesz;
			ELF_PRINTF("loaded segment to %x\n", get_load_addr(&phdr[i]));
			if (pad_space) {
				memset(pad_start, 0, pad_space);
				ELF_PRINTF("cleared %x-%x\n", pad_start, pad_start + pad_space);
			}
		}
	}
}
#endif

/* elf_loadN: Load an executable elf.
 *
 * In order to make sure this function is "exporting" everything correctly to the
 * operating system, to build the multiboot structure, the function takes the same
 * arguments that the OS would get from this bootloader. This is currently only
 * tailored and tested on HOBos kernels, but it can potentially work on any elf, I
 * guess.
 *
 * @addr: The dereferencable address of the entire [unloaded] ELF image
 * @shdr: Section headers
 * @shnum: Number of sections
 * @shstrndx: String table for section names
 * @ret: Output for the multiboot info
 *
 * Returns the address of the ELF's entry point.
 */
#define elf_loadN __CAT(elf_load, __ELF_WORD_SIZE)
__ElfN(Addr) elf_loadN(const void *addr,
					   __ElfN(Shdr) *shdr,
					   int shnum,
					   int shstrndx,
					   struct multiboot_elf_section_header_table *ret) {
	int i;

	__ElfN(Ehdr) *ehdr = (__ElfN(Ehdr) *)addr;
	if (ehdr == 0)
		return 0;

	__ElfN(Phdr) *phdr = (__ElfN(Phdr) *)(addr + ehdr->e_phoff);
	ASSERT(phdr != 0);

	ASSERT(ehdr->e_phentsize == sizeof(__ElfN(Phdr)));
	__ElfN(Half) phdr_count = ehdr->e_phnum;

	load_segments(addr, phdr, phdr_count);

	/* In order to get symbols, we must parse the section headers (and modify them
	 * appropriately. This seems hackish to me, but it is what GRUB/Multiboot does.
	 */
	for (i = 0; i < shnum; i++) {
		__ElfN(Shdr) *sh = &shdr[i];
		if (sh->sh_size == 0)
			continue;

		if (sh->sh_addr) /* Already loaded */
			continue;

		ASSERT(sizeof(void *) == 4);
		*((volatile __ElfN(Addr) *)&sh->sh_addr) = sh->sh_offset + (uint32_t)addr;
	}

	ret->num = (multiboot_uint32_t)ehdr->e_shnum;
	ret->size = (multiboot_uint32_t)ehdr->e_shentsize;
	ret->addr = (multiboot_uint32_t)ehdr->e_shoff + (multiboot_uint32_t)addr;
	ret->shndx = (multiboot_uint32_t)ehdr->e_shstrndx;

	return ehdr->e_entry;
}
