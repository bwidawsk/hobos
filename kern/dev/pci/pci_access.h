#ifndef _PCI_ACCESS_H_
#define _PCI_ACCESS_H_

#define PCI_OFF_SHIFT 2
#define PCI_FUNC_SHIFT 8
#define PCI_DEV_SHIFT 11
#define PCI_BUS_SHIFT 16

#define PCI_MAX_BUS 255
#define PCI_BUS_MASK 0xFF
#define PCI_MAX_DEV 31
#define PCI_DEV_MASK 0x1f
#define PCI_MAX_FUNC 7
#define PCI_FUNC_MASK 0x7
#define PCI_OFF_MASK 0xFF
#define PCI_MAX_IDS (PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC)
#define PCI_BDFO(bus, dev, func, offset) ((bdfo_t)(bus << 16  |  (dev << 11)  |  (func <<  8) | offset))
#define PCI_BDFO_BUS(bdfo) ((bdfo >> 16) & PCI_BUS_MASK)
#define PCI_BDFO_DEV(bdfo) ((bdfo >> 11) & PCI_DEV_MASK)
#define PCI_BDFO_FUNC(bdfo) ((bdfo >> 8) & PCI_FUNC_MASK)
#define PCI_BDFO_OFF(bdfo) ((bdfo) & PCI_OFF_MASK)

#define FOREACH_PCI_DEV(bus, dev, func, temp) \
	for(bus = 0, dev = 0, func = 0, temp = 0; \
		temp < PCI_MAX_IDS ; \
		func = (temp % 8), \
		dev = ((temp) >> 3) & PCI_DEV_MASK, \
		bus = ((temp) >> 8) & PCI_BUS_MASK, \
		temp++)

typedef uint8_t pci_class_t;
typedef uint32_t bdfo_t;

extern struct pci_accessor *pci;

/* Structure doesn't need to be packed since it isn't directly read */
struct pci_header {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision;
	uint8_t prog_interface;
	uint8_t subclass_code;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency;
	uint8_t header;
	uint8_t bist;
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	uint32_t CBus_ptr;
	uint16_t sub_vendor_id;
	uint16_t sub_id;
	uint32_t erom_addr;
	uint32_t rsvd1;
	uint32_t rsvd2;
	uint8_t irq;
	uint8_t pin;
	uint8_t min_grant; 
	uint8_t max_grant;
} ; 

#define PCI_VENID_OFF 0x0
#define PCI_DEVID_OFF 0x2
#define PCI_CMD_OFF 0x4

#define PCI_STATUS_OFF 0x6
#define PCI_REVID_OFF 0x8
#define PCI_PROGIF_OFF 0x9
#define PCI_SUBCLASS_OFF 0xa
#define PCI_CLASS_OFF 0xb
#define PCI_CACHE_OFF 0xc
#define PCI_LATENCY_OFF 0xd
#define PCI_HEADER_OFF 0xe
#define PCI_BIST_OFF 0xf
#define PCI_BAR0_OFF 0x10
#define PCI_BAR1_OFF 0x14
#define PCI_BAR2_OFF 0x18
#define PCI_BAR3_OFF 0x1C
#define PCI_BAR4_OFF 0x20
#define PCI_BAR5_OFF 0x24
#define PCI_CBUS_OFF 0x28
#define PCI_SUBVID_OFF 0x2C
#define PCI_SUBID_OFF 0x2E
#define PCI_EROM_OFF 0x30
#define PCI_RSVD1_OFF 0x34
#define PCI_RSVD2_OFF 0x38
#define PCI_IRQ_OFF 0x3C
#define PCI_PIN_OFF 0x3D
#define PCI_MING_OFF 0x3E
#define PCI_MAXG_OFF 0x3F

#define PCI_CLASS_NOCLASS 0x00	 // Device was built prior definition of the class code field
#define PCI_CLASS_MASS_STORAGE 0x01	 // Mass Storage Controller
#define PCI_CLASS_NIC 0x02	 // Network Controller
#define PCI_CLASS_DISPLAY 0x03	 // Display Controller
#if 0
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

static char *pci_classes[] __attribute__((used)) = {
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

uint8_t  pci_read8(bdfo_t bdfo);
uint16_t pci_read16(bdfo_t bdfo);
uint32_t pci_read32(bdfo_t bdfo);
void pci_write8(bdfo_t bdfo, uint8_t data);
void pci_write16(bdfo_t bdfo, uint16_t data);
void pci_write32(bdfo_t bdfo, uint32_t data);
int pci_read_header(bdfo_t which, struct pci_header *cfg_header);
int pci_get_devs(pci_class_t, bdfo_t *ret, int *count);



#endif
