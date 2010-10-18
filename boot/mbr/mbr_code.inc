.set MBR_VER, 0x43 			# character representing version
.set RELO_ADDR, (0x7a00)	# someplace where our code can execute (we want to load the next stage into 0x7c00)
.set PART_OFF, (0x1be)		# offset into MBR where partition table lives
.set PARTITION_TABLE_START, (RELO_ADDR+PART_OFF) #lva of partition table
.set PARTITION_TABLE_ENTRY_SIZE, (16)
.set PARTITION_TABLE_SIZE, (PARTITION_TABLE_ENTRY_SIZE * 4)
.set PARTITION_TABLE_END, (PARTITION_TABLE_START + PARTITION_TABLE_SIZE)
.set PARTITION_MAGIC, (0xaa55)
.set MAGIC_OFFSET, (0x1fe)
.set MBR_SIZE, 0x200
.set VBR_LOAD_ADDR, 0x7c00
.set BOOTABLE_FLAG, 0x80

.code16
	cli

# Initialize segments to 0, but our stack below where our code will relocate
# to.
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %ss
	movw $RELO_ADDR, %sp

	#copy ourselves so we can put the VBR at 0x7c00
	cld
	movw $VBR_LOAD_ADDR, %si
	movw $RELO_ADDR, %di
	movw $MBR_SIZE, %cx
	rep movsb

	# ret to our relocated self
	# TODO: make this an ljmp, I think it's less bytes
	lea	 MAIN, %si
	push %cs
	push %si
	ret


MAIN:
	mov $MBR_VER, %ax
	call putc
	movb $0, %dh
	movb $2, %cl
	movb $0, %ch
	movw $VBR_LOAD_ADDR, %bx
	call read_sector
	testw %ax, %ax
	je bad

	movw $PARTITION_TABLE_START, %si

	movw 0x7c00, %cx
	cmpw $(0xB00B), %cx
	jne search_partition_entries
	movw 0x7c02, %cx
	cmpw $(0xDEAD), %cx
	jne search_partition_entries
	jmp found_benboot


search_partition_entries:
	movb (%si), %al
	test $BOOTABLE_FLAG, %al
	jnz found
	add $PARTITION_TABLE_ENTRY_SIZE, %si
	cmpw $PARTITION_TABLE_END, %si
	jge bad
	jmp search_partition_entries


found:
# for now we assume we load one sector
# reading: head = byte 1, sector = byte 2[5:0], cyl = byte2[7:6] byte3[7:0]
# for int 0x13, cx = cylinder << 6 | sector, dl = drive, dh=head
	movb 1(%si), %dh
	movb 2(%si), %cl
	movb 3(%si), %ch
	movw $VBR_LOAD_ADDR, %bx
	call read_sector
	testw %ax, %ax
	je bad

# check for aa55 magic in the loaded vbr
	cmpw $PARTITION_MAGIC, (VBR_LOAD_ADDR + MAGIC_OFFSET)
	jne bad

found_benboot:
	# in case we're chainloading, we need to have 3 things
	# cs:ip = 0:7c00
	# dl = BIOS drive number we loaded
	# si = pointer to partition table
	jmp VBR_LOAD_ADDR+4

bad:
	movb $'B', %al
	call putc
	movb $'A', %al
	call putc
	movb $'D', %al
	call putc
	cli
	hlt

# All legacy BIOS info came from Ralph Brown list.
putc:
	pusha
	xorw    %bx, %bx        # page 0, color 0
    movb    $0x0e, %ah
    int     $0x10
	popa
   	ret

#CH = Track
#CL = Sector
#DH = Head
#DL = Drive
#ES:BX buffer
#AX = 0 success; 1 fail
read_sector:
        pusha
        movw $0x201, %ax
        int $0x13
        xorw %ax, %ax
        jnc read_sector_pass
        mov $1, %ax
read_sector_pass:
        popa
        ret

