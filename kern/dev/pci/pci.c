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
pci_get_devs(pci_class_t class, bdfo_t *ret) {

	return 0;
}

