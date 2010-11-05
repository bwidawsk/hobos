#include <stdint.h>
#include "pci_access.h"

uint8_t
pci_read8(bdfo_t bdfo) {
	return pci->pci_read8(bdfo);
}

uint16_t
pci_read16(bdfo_t bdfo) {
	return pci->pci_read16(bdfo);
}
uint32_t 
pci_read32(bdfo_t bdfo) {
	return pci->pci_read32(bdfo);
}

void 
pci_write8(bdfo_t bdfo, uint8_t data) {
	pci_write8(bdfo, data);

}
void 
pci_write16(bdfo_t bdfo, uint16_t data) {
	pci_write16(bdfo, data);
}

void 
pci_write32(bdfo_t bdfo, uint32_t data) {
	pci_write32(bdfo, data);
}



int 
pci_get_devs(pci_class_t class, bdfo_t *ret, int *count) {
	uint16_t bus = 0, dev = 0, func = 0, temp = 0;
	FOREACH_PCI_DEV(bus, dev, func, temp) {

	}
	return 0;
}

#include <bs_commands.h>
static void *
generic_lspci(int argc, char *argv[]) {

	uint16_t bus = 0, dev = 0, func = 0, temp = 0;
	FOREACH_PCI_DEV(bus, dev, func, temp) {
		uint16_t devid = pci_read16(PCI_BDFO(bus, dev, func, PCI_DEVID_OFF));
		if(devid != 0xFFFF) {
			printf("\n%s\n", pci_classes[pci_read8(PCI_BDFO(bus, dev, func, PCI_CLASS_OFF))]);
			printf("\t%02x %02x %02x %02x\n\t%02x %02x %02x %02x",
			pci_read8(PCI_BDFO(bus,dev,func, 0)), 
			pci_read8(PCI_BDFO(bus,dev,func, 1)), 
			pci_read8(PCI_BDFO(bus,dev,func, 2)), 
			pci_read8(PCI_BDFO(bus,dev,func, 3)), 
			pci_read8(PCI_BDFO(bus,dev,func, 4)), 
			pci_read8(PCI_BDFO(bus,dev,func, 5)), 
			pci_read8(PCI_BDFO(bus,dev,func, 6)), 
			pci_read8(PCI_BDFO(bus,dev,func, 7)));
		}
	}
	return NULL;
}

static void
pci_help() {
	printf("Show the pci bus\n");
}

BS_COMMAND_DECLARE(lspci, generic_lspci, pci_help);
