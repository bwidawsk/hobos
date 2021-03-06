#include <arch/asm.h>
#include "pci_access.h"

static uint32_t x86_io_pci_read32(bdfo_t bdfo);

#define PCI_CFG_ACC1(bdf0) ((bdfo |= (1 << 31)) & 0x80FFFFFC)

static
uint8_t x86_io_pci_read8(bdfo_t bdfo) {
#if NO
	outl(PCI_CFG_ACC1(bdfo), 0xcf8);
	return inb(0xCFC);
#else
	uint32_t temp = x86_io_pci_read32(bdfo);
	return (temp >> ((PCI_BDFO_OFF(bdfo) & 3) * 8)) & 0xFF;
#endif
}

static
uint16_t x86_io_pci_read16(bdfo_t bdfo) {
#if NO
	outl(PCI_CFG_ACC1(bdfo), 0xcf8);
	return inw(0xCFC);
#else
	uint32_t temp = x86_io_pci_read32(bdfo);
	return (temp >> ((PCI_BDFO_OFF(bdfo) & 2) * 8)) & 0xFFFF;
#endif
}

static uint32_t 
x86_io_pci_read32(bdfo_t bdfo) {
	outl(PCI_CFG_ACC1(bdfo), 0xcf8);
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
	outl(PCI_CFG_ACC1(bdfo), 0xcf8);
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
/* TODO: this shouldn't be here or hardcoded */
struct pci_accessor *pci = &x86_io_pci;
