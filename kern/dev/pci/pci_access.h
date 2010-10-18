#ifndef _PCI_ACCESS_H_
#define _PCI_ACCESS_H_

typedef uint32_t bdfo_t;

#define PCI_BDFO(b, d, f, o) (bdfo_t)(b << 16  |  d << 11  |  f <<  8 | o)
#define BDFO_BUS(bdfo) ((bdfo >> 16) & 0xFFFF)
#define BDFO_DEV(bdfo) ((bdfo >> 11) & 0x3F)
#define BDFO_FUNC(bdfo) ((bdfo >> 3) & 0x7)
#define BDFO_oFF(bdfo) ((bdfo >> 0) & 0xFF)

struct pci_accessor {
	uint8_t  (*pci_read8)(bdfo_t bdfo);
	uint16_t (*pci_read16)(bdfo_t bdfo);
	uint32_t (*pci_read32)(bdfo_t bdfo);
	void (*pci_write8)(bdfo_t bdfo, uint8_t data);
	void (*pci_write16)(bdfo_t bdfo, uint16_t data);
	void (*pci_write32)(bdfo_t bdfo, uint32_t data);
};

extern struct pci_accessor *pci;

#endif