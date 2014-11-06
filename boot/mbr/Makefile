
# All disk related stuff assumes the following for now:
# 2 heads per platter, 8 platter, 63 SPT, and we want to fill 2 cylinders

all : mbr.img

mbr.o : mbr.S mbr_code.inc
	as mbr.S -o mbr.o

mbr.img : mbr.o
	ld -N -e entry -T mbr.lds -S --oformat binary mbr.o -o mbr.img
	ld -N -e entry -T mbr.lds -S mbr.o -o mbr.elf

mbr.lst :
	objdump --disassemble -mi8086 mbr.elf


clean:
	-@rm -f mbr.o mbr.img mbr.elf
