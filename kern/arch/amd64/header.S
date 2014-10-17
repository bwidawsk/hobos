/*
 * BW note: This original came from a FSF website. It's been modified
 * by me. The website didn't have a license that I saw specifically,
 * but I should go back and check.
 */

#define ASM_FILE        1
#include "multiboot.h"
#include "gdt.h"
#include "../ia_common/ia_defines.h"

/* The size of our stack (16KB). */
#define STACK_SIZE                      0x4000

/* The flags for the Multiboot header. */

#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

.code32
.section .multiboot, "ax"
.globl  start, _start
start:
_start:
jmp     multiboot_entry

/* Align 32 bits boundary. */
.align  4

/* Multiboot header. */
multiboot_header:
/* magic */
.long   MULTIBOOT_HEADER_MAGIC
/* flags */
.long   MULTIBOOT_HEADER_FLAGS
/* checksum */
.long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
/* header_addr */
.long   multiboot_header
/* load_addr */
.long   _start
/* load_end_addr */
.long   _edata
/* bss_end_addr */
.long   _end
/* entry_addr */
.long   multiboot_entry
#endif /* ! __ELF__ */

multiboot_entry:
/*
 * We don't want to trust the stack that was given to us. Once
 * we do this, we can no longer reliably return to the bootloader.
 */
//movl    $(stack + STACK_SIZE), %esp

/* Reset EFLAGS. BW: for some reason OS doesn't trust EFLAGS */
pushl   $0
popf

/* Push the multiboot info struct */
pushl   %ebx
/* Push the magic value. */
pushl   %eax

jmp goto_64


goto_64:
/* Don't clobber eax, and ebx which have stuff we care about */
/* pointer to the Multiboot information structure. */
movl %ebx, %edi
/* multiboot magic */
movl %eax, %esi

/* TODO: we should check some basic cpuid values so something sane
 * happens in bad case
 */

/* SDM Voluma 3A: 4.1.2 & 9.8.5:
 * 64 bit long mode needs PAE + EFER.LME + a valid CR3 + CR0.PG 
 * For CR3 we use statically allocated, identity mapped page tables.
 */

/* 9.8.5 #1 Disable paging */
movl %cr0, %eax
btrl $31, %eax
movl %eax, %cr0

/* 9.8.5 #2 Enable PAE */
movl %cr4, %eax
or $CR4_PAE, %eax
movl %eax, %cr4

/* 9.8.5 #3 Load cr3 with PA of PML4a */
lea early_pml4e, %eax
movl %eax, %cr3

/* 9.8.5 #4 Set IA32_EFER.LME */
movl $IA32_EFER, %ecx
rdmsr
or $IA32_EFER_LME, %eax
wrmsr

/* 9.8.5 #5 Enable paging */
movl %cr0, %eax
or $CR0_PG, %eax #enable paging
movl %eax, %cr0

/* Now long jump into the 64bit code segment */
lgdt (gdt_desc)
ljmp $KERNEL_CS64, $place_to_jump

place_to_jump:
movl $KERNEL_DS, %eax
movl %eax, %ds
movl %eax, %es
movl %eax, %fs
movl %eax, %gs
movl %eax, %ss

.code64
/* We earlier moved multiboot pointer and magic to our arguments
 * begin has not yet fixed up page tables, so we should call from
 * the phsyical address
 */
movq $amd64_begin, %rax
call *%rax

gdtdesc:
/* TODO: how to unhardcode this??? we picked 5f at random*/
gdt_desc:
.word 0x5f
.long mboot_gdt
