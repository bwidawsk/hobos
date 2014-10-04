#ifndef _ELF_HEADER_H_
#define _ELF_HEADER_H_

#include "elf_common.h"
#include "elf_data_types.h"

/* TODO: FreeBSD license */
#if __ELF_WORD_SIZE != 32 && __ELF_WORD_SIZE != 64
#error "__ELF_WORD_SIZE must be defined as 32 or 64"
#endif

#define ELF_CLASS   __CAT(ELFCLASS,__ELF_WORD_SIZE)

#if BYTE_ORDER == LITTLE_ENDIAN
#define ELF_DATA    ELFDATA2LSB
#elif BYTE_ORDER == BIG_ENDIAN
#define ELF_DATA    ELFDATA2MSB
#else
#error "Unknown byte order"
#endif

#define __elfN(x)   __CAT(__CAT(__CAT(elf,__ELF_WORD_SIZE),_),x)
#define __ElfN(x)   __CAT(__CAT(__CAT(Elf,__ELF_WORD_SIZE),_),x)
#define __ELFN(x)   __CAT(__CAT(__CAT(ELF,__ELF_WORD_SIZE),_),x)
#define __ElfType(x)    typedef __ElfN(x) __CAT(Elf_,x)

/* TODO: END FreeBSD license */
typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

typedef struct {
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

typedef struct
{
	Elf64_Word p_type; /* Type of segment */
	Elf64_Word p_flags; /* Segment attributes */
	Elf64_Off p_offset; /* Offset in file */
	Elf64_Addr p_vaddr; /* Virtual address in memory */
	Elf64_Addr p_paddr; /* Reserved */
	Elf64_Xword p_filesz; /* Size of segment in file */
	Elf64_Xword p_memsz; /* Size of segment in memory */
	Elf64_Xword p_align; /* Alignment of segment */
} Elf64_Phdr;

typedef struct
{
	Elf64_Word st_name; /* Symbol name */
	unsigned char st_info; /* Type and Binding attributes */
	unsigned char st_other; /* Reserved */
	Elf64_Half st_shndx; /* Section table index */
	Elf64_Addr st_value; /* Symbol value */
	Elf64_Xword st_size; /* Size of object (e.g., common) */
} Elf64_Sym;
#endif //_ELF_HEADER_H_
