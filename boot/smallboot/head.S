#our primary goal is to get into 32bit protected mode so we can do some ext2
# stuff and retrieve our kernel

.long 0xdeadb00b

.code16
.globl start
start:
	# We expect dl = drive, cs:ip = 0x7c00, si = partition table entry we're
	# loaded from

	# Let's get into 32 bit protected modea

	mov %cr0, %ebx
	or $1, %bx
	mov %ebx, %cr0
	lgdt (gdt_desc)
	ljmp $0x8,$protected_mode

.code32
protected_mode:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	#fastcall, arg1 = ecx, arg=edx
	movzbl %dl, %ecx
	movzwl %si, %edx
	call bootstrap


.globl gdt_desc
gdt_desc:
	.word egdt - gdt - 1
	.long gdt

.align 8
gdt:
# 0 segment
.long 0, 0
#code (0x8)
.word 0xffff		#limit 15:0
.word 0x0		#base 15:0
.byte 0x0		#base 23:16
.byte 0x9e		#present, ring 0, executable, conforming, readable, !accessed
.byte 0xcf		#granularity size, limit[19:16] = f
.byte 0x0		#base 31:24
#data (0x10)
.word 0xffff		#limit 15:0
.word 0x0		#base 15:0
.byte 0x0		#base 23:16
.byte 0x92		#present, priv=0, !executable, stack down, writable, !accessed
.byte 0xcf		#granularity=1 size=1, limit 19:16 = f
.byte 0x0		#base 31:24
# we also have 16 bit code and data to do real mode BIOS calls
#code (0x18)
.word 0xffff		#limit 15:0
.word 0x0		#base 15:0
.byte 0x0		#base 23:16
.byte 0x9e		#present, ring 0, executable, conforming, readable, !accessed
.byte 0x8f		#granularity size, limit[19:16] = f
.byte 0x0		#base 31:24
#data (0x20)
.word 0xffff		#limit 15:0
.word 0x0		#base 15:0
.byte 0x0		#base 23:16
.byte 0x92		#present, priv=0, !executable, stack down, writable, !accessed
.byte 0x8f		#granularity=1 size=1, limit 19:16 = f
.byte 0x0		#base 31:24
egdt:

