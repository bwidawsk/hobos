.code32
/*
 * intX is a generic function for doing real mode interrupts
 * it doesn't properly return CF, but we don't care about that yet
 * no fs/gs to save room
 */
.global intX
# the object here is to get us to 16 bit mode, and do the int13
intX:
	pushl %ebx
	movl 8(%esp), %ebx
	pushl %esi
	lea tramp_opcode, %esi
	mov %bl, 1(%esi)
	popl %esi


	# 16 bit data selector into eax, reset descriptors
	# remember what we clobber (bx, si)
	movw $0x20, %bx
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %ss

	ljmp $0x18,$legacy_interrupt

.global trampoline
trampoline:
	movw $0x10, %bx
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %ss
	pushfl
	popl %ebx
	movl %ebx, 8(%esp)
	popl %ebx
	ret

.code16gcc
legacy_interrupt:
	mov %cr0, %ebx
	and $0xFFFFFFFE, %ebx
	mov %ebx, %cr0
	ljmp $0x0,$legacy_interrupt_real

legacy_interrupt_real:
	xorw %bx, %bx
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %ss
	lidt (real_mode_idt)
	#restore our bx now because we might need it
	movl (%esp), %ebx
	add $4, %esp

.globl tramp_opcode
tramp_opcode:
#	int ??$
	.byte 0xcd
	.byte 0x00

	push %ebx

	movl %cr0, %ebx
	orw  $0x1, %bx
	movl %ebx, %cr0
	lgdt (gdt_desc)
	ljmp $0x8,$trampoline


real_mode_idt:
	.short 0x3ff
	.long 0x0
