#ifndef _ELF_DATA_TYPES_H_
#define _ELF_DATA_TYPES_H_

/* Size Alignment  Desc*/
typedef uint32_t Elf32_Addr;	/* 4	Unsigned program address */
typedef uint16_t Elf32_Half;	/* 2	Unsigned medium integer  */
typedef uint32_t Elf32_Off;		/* 4	Unsigned file offset*/
typedef int32_t  Elf32_Sword;	/* 4	Signed large integer*/
typedef uint32_t Elf32_Word;	/* 4	Unsigned large integer */

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef int16_t  Elf64_SHalf;
typedef uint64_t Elf64_Off;
typedef int32_t  Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

#define Elf_Addr __ElfN(Addr)
#define Elf_Half __ElfN(Half)
#define Elf_Off __ElfN(Off)
#define Elf_Sword __ElfN(Sword)
#define Elf_Word __ElfN(Word)

#endif // _ELF_DATA_TYPES_H_
