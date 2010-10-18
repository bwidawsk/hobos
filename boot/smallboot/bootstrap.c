#include "bios.h"
#include "legacy_int.h"

/* Function name of the loaded (stage 1.5 in grub) bootloader */
extern void main(int size, unsigned char drive_letter, struct partition *partition_entry) __attribute__((noreturn));

/* We used to have a bss and clear it, but we don't anymore */
extern unsigned char _bstart, _bend, _btext, _edata;

/*
 * We enter here in 32 bit protected mode and 0x6a00  (sp=0x7a00 - 0x1000 for IVT) of stack space.
 * Interrupts are turned off.
 * For now we just want to load the remaining sectors of our 1.5 stage bootloader.
 * We make at least the following assumptions which should be abstracted later
 * 	1. sector size of the load medium is 512b
 * 	2. x86 legacy interrupts can read the load medium
 */
void __attribute__ ((fastcall))
bootstrap(unsigned char drive_letter, struct partition *partition_entry) {
	/* 
	 * divide by 512 to find the number of sectors just add one to be safe 
	 * instead of really doing the math
	 */
	int num_sectors = ((&_edata - &_btext) / 512) + 1;
	//int sectors[2] = {2,0};
	
	/* use legacy interrupt 0x13/ah=2 because we on't know what we have */
	/*
	struct legacy_regs_32 regs = {
		.eax = 0x0800,
		.edx = drive_letter
	};
	legacy_int(0x13, &regs);
	//ASSERT(!(regs.cf & 1));
	unsigned char heads = regs.edx >> 8;
	unsigned short cylinders = regs.ecx >> 6;
	unsigned char spt = regs.ecx & 0x3f;
	*/
	// we know we want to start reading at LBA 2, which should always be CHS 003
	const int cyl = 0;
	const int sec = 3;
	const int head = 0;
	struct legacy_regs_32 regs;
	regs.eax = (0x2 << 8) | num_sectors;
	regs.ecx = ((cyl & 255) << 8) | ((cyl & 0x300) >> 2) | sec;
	regs.edx = (head << 8) | drive_letter;
	regs.ebx = 0x7e00;
	
	legacy_int(0x13, &regs);
	//read_sector((void *)0x7e00, num_sectors, sectors, drive_letter);

	/* pass the read_sector function onto main so it can load files */
	main(num_sectors * 512, drive_letter, partition_entry);
}