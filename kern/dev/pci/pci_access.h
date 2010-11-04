#ifndef _PCI_ACCESS_H_
#define _PCI_ACCESS_H_

typedef uint32_t bdfo_t;

#define PCI_BDFO(bus, dev, func, offset) ((bdfo_t)(bus << 16  |  (dev << 11)  |  (func <<  8) | offset))
	
#define BDFO_BUS(bdfo) ((bdfo >> 16) & 0xFFFF)
#define BDFO_DEV(bdfo) ((bdfo >> 11) & 0x3F)
#define BDFO_FUNC(bdfo) ((bdfo >> 3) & 0x7)
#define BDFO_OFF(bdfo) ((bdfo >> 0) & 0xFF)

#define PCI_MAX_BUS 256
#define PCI_MAX_DEV 64
#define PCI_MAX_FUNC 8

#define PCI_VENID_OFF 0x0
#define PCI_DEVID_OFF 0x2
#define PCI_CMD_OFF 0x4

#define PCI_STATUS_OFF 0x6
#define PCI_REVID_OFF 0x8
#define PCI_PROGIF_OFF 0x9
#define PCI_SUBCLASS_OFF 0xa
#define PCI_CLASS_OFF 0xb

#if 0
#define PCI_CLASS_NOCLASS 0x00	 // Device was built prior definition of the class code field
#define PCI_CLASS_MASS_STORAGE 0x01	 // Mass Storage Controller
#define PCI_CLASS_NIC 0x02	 // Network Controller
#define PCI_CLASS_DISPLAY 0x03	 // Display Controller
#define PCI_CLASS_ 0x04	 // Multimedia Controller
#define PCI_CLASS_ 0x05	 // Memory Controller
#define PCI_CLASS_ 0x06	 // Bridge Device
#define PCI_CLASS_ 0x07	 // Simple Communication Controllers
#define PCI_CLASS_ 0x08	 // Base System Peripherals
#define PCI_CLASS_ 0x09	 // Input Devices
#define PCI_CLASS_ 0x0A	 // Docking Stations
#define PCI_CLASS_ 0x0B	 // Processors
#define PCI_CLASS_ 0x0C	 // Serial Bus Controllers
#define PCI_CLASS_ 0x0D	 // Wireless Controllers
#define PCI_CLASS_ 0x0E	 // Intelligent I/O Controllers
#define PCI_CLASS_ 0x0F	 // Satellite Communication Controllers
#define PCI_CLASS_ 0x10	 // Encryption/Decryption Controllers
#define PCI_CLASS_ 0x11	 // Data Acquisition and Signal Processing Controllers
#define PCI_CLASS_ 0x12 // - 0xFE	 Reserved
#define PCI_CLASS_ 0xFF	 // Device does not fit any defined class.
#endif

#define PCI_MASS_STORAGE 1

static char *pci_classes[] = {
	"Device was built prior definition of the class code field",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"Bridge Device",
	"Simple Communication Controllers",
	"Base System Peripherals",
	"Input Devices",
	"Docking Stations",
	"Processors",
	"Serial Bus Controllers",
	"Wireless Controllers",
	"Intelligent I/O Controllers",
	"Satellite Communication Controllers",
	"Encryption/Decryption Controllers",
	"Data Acquisition and Signal Processing Controllers",
	"- 0xFE	 Reserved",
	"Device does not fit any defined class."
};

struct pci_accessor {
	uint8_t  (*pci_read8)(bdfo_t bdfo);
	uint16_t (*pci_read16)(bdfo_t bdfo);
	uint32_t (*pci_read32)(bdfo_t bdfo);
	void (*pci_write8)(bdfo_t bdfo, uint8_t data);
	void (*pci_write16)(bdfo_t bdfo, uint16_t data);
	void (*pci_write32)(bdfo_t bdfo, uint32_t data);
};

extern struct pci_accessor *pci;

uint8_t  pci_read8(bdfo_t bdfo);
uint16_t pci_read16(bdfo_t bdfo);
uint32_t pci_read32(bdfo_t bdfo);
void pci_write8(bdfo_t bdfo, uint8_t data);
void pci_write16(bdfo_t bdfo, uint16_t data);
void pci_write32(bdfo_t bdfo, uint32_t data);

typedef uint8_t pci_class_t;

int pci_get_devs(pci_class_t, bdfo_t *ret);

#endif
