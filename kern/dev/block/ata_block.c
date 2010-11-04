#include <stdint.h>
#include <dev/pci/pci_access.h>
#include <mm/malloc.h>
#include "block.h"

ata_scan_devs() {
	void *temp = malloc(sizeof(bdfo_t) * PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC);
	pci_get_devs(PCI_MASS_STORAGE, temp);
}
