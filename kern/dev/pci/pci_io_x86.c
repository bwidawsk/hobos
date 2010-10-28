#include <stdint.h>
#include <arch/asm.h>
#include "pci_access.h"

static uint32_t x86_io_pci_read32(bdfo_t bdfo);

static
uint8_t x86_io_pci_read8(bdfo_t bdfo) {
#if NO
	outl((1 << 31) | (bdfo), 0xcf8);
	return inb(0xCFC);
#else
	uint32_t temp = x86_io_pci_read32(bdfo & 0xFFFFFFFC);
	return (temp >> ((bdfo & 3) * 8));
#endif
}

static
uint16_t x86_io_pci_read16(bdfo_t bdfo) {
#if NO
	outl((1 << 31) | (bdfo), 0xcf8);
	return inw(0xCFC);
#else
	uint16_t ret;
	uint32_t temp = x86_io_pci_read32(bdfo & 0xFFFFFFFC);
	return (temp >> ((bdfo & 3) * 8));
#endif
}

static uint32_t 
x86_io_pci_read32(bdfo_t bdfo) {
	outl((1 << 31) | (bdfo), 0xcf8);
	return inl(0xCFC);
}

static
void x86_io_pci_write8(bdfo_t bdfo, uint8_t data) {
	outl((1 << 31) | (bdfo), 0xcf8);
	outb(data, 0xcfc);
}

static
void x86_io_pci_write16(bdfo_t bdfo, uint16_t data) {
	outl((1 << 31) | (bdfo), 0xcf8);
	outw(data, 0xcfc);
}

static
void x86_io_pci_write32(bdfo_t bdfo, uint32_t data) {
	outl((1 << 31) | (bdfo), 0xcf8);
	outl(data, 0xcfc);
}

struct pci_accessor x86_io_pci = {
	.pci_read8 = x86_io_pci_read8,
	.pci_read16 = x86_io_pci_read16,
	.pci_read32 = x86_io_pci_read32,
	.pci_write8 = x86_io_pci_write8,
	.pci_write16 = x86_io_pci_write16,
	.pci_write32 = x86_io_pci_write32
};


/* TODO: this belongs elsewhere */
#include <bs_commands.h>
static void *
x86_io_lspci(int argc, char *argv[]) {
	uint16_t bus = 0;

	for(bus = 0; bus < 256; bus++) {
		uint8_t devno = 0;
		for(devno = 0; devno < 16; devno++) {
			uint8_t func = 0;
			for(func = 0; func < 8; func++) {
				uint16_t devid = x86_io_pci_read16(PCI_BDFO(bus, devno, func, PCI_DEVID_OFF));
				if(devid != 0xFFFF) {
					printf("\n%s\n", pci_classes[x86_io_pci_read8(PCI_BDFO(bus, devno, func, PCI_CLASS_OFF))]);
					printf("\t%02x %02x %02x %02x\n\t%02x %02x %02x %02x",
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 0)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 1)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 2)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 3)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 4)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 5)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 6)), 
						x86_io_pci_read8(PCI_BDFO(bus,devno,func, 7)));
				}
				/*
				if(devid != 0xFFFF) {
					printf("\n%x\n%x\n",
						x86_io_pci_read32(PCI_BDFO(bus,devno,func, 0)), 
						x86_io_pci_read32(PCI_BDFO(bus,devno,func, 4)));
				}*/
			}
		}
	}
	return NULL;
}

static void
x86_io_pci_help() {
	printf("Show the pci bus\n");
}

BS_COMMAND_DECLARE(lspci, x86_io_lspci, x86_io_pci_help);
