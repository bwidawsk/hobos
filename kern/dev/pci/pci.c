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
pci_read_header(bdfo_t which, struct pci_header *cfg_header) {
	uint8_t bus = PCI_BDFO_BUS(which);
	uint8_t dev = PCI_BDFO_DEV(which);
	uint8_t func = PCI_BDFO_FUNC(which);
	cfg_header->vendor_id = pci_read16(PCI_BDFO(bus, dev, func, PCI_VENID_OFF));
	cfg_header->device_id = pci_read16(PCI_BDFO(bus, dev, func, PCI_DEVID_OFF));
	cfg_header->command = pci_read16(PCI_BDFO(bus, dev, func, PCI_CMD_OFF));
	cfg_header->status = pci_read16(PCI_BDFO(bus, dev, func, PCI_STATUS_OFF));
	cfg_header->revision = pci_read8(PCI_BDFO(bus, dev, func, PCI_REVID_OFF));
	cfg_header->prog_interface = pci_read8(PCI_BDFO(bus, dev, func, PCI_PROGIF_OFF));
	cfg_header->subclass_code = pci_read8(PCI_BDFO(bus, dev, func, PCI_SUBCLASS_OFF));
	cfg_header->class_code = pci_read8(PCI_BDFO(bus, dev, func, PCI_CLASS_OFF));
	cfg_header->cache_line_size = pci_read8(PCI_BDFO(bus, dev, func, PCI_CACHE_OFF));
	cfg_header->latency = pci_read8(PCI_BDFO(bus, dev, func, PCI_LATENCY_OFF));
	cfg_header->header = pci_read8(PCI_BDFO(bus, dev, func, PCI_HEADER_OFF));
	cfg_header->bist = pci_read8(PCI_BDFO(bus, dev, func, PCI_BIST_OFF));
	cfg_header->bar0 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR0_OFF));
	cfg_header->bar1 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR1_OFF));
	cfg_header->bar2 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR2_OFF));
	cfg_header->bar3 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR3_OFF));
	cfg_header->bar4 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR4_OFF));
	cfg_header->bar5 = pci_read32(PCI_BDFO(bus, dev, func, PCI_BAR5_OFF));
	cfg_header->CBus_ptr = pci_read32(PCI_BDFO(bus, dev, func, PCI_CBUS_OFF));
	cfg_header->sub_vendor_id = pci_read16(PCI_BDFO(bus, dev, func, PCI_SUBVID_OFF));
	cfg_header->sub_id = pci_read16(PCI_BDFO(bus, dev, func, PCI_SUBID_OFF));
	cfg_header->erom_addr = pci_read32(PCI_BDFO(bus, dev, func, PCI_EROM_OFF));
	cfg_header->rsvd1 = pci_read32(PCI_BDFO(bus, dev, func, PCI_RSVD1_OFF));
	cfg_header->rsvd2 = pci_read32(PCI_BDFO(bus, dev, func, PCI_RSVD2_OFF));
	cfg_header->irq = pci_read8(PCI_BDFO(bus, dev, func, PCI_IRQ_OFF));
	cfg_header->pin = pci_read8(PCI_BDFO(bus, dev, func, PCI_PIN_OFF));
	cfg_header->min_grant = pci_read8(PCI_BDFO(bus, dev, func, PCI_MING_OFF));
	cfg_header->max_grant = pci_read8(PCI_BDFO(bus, dev, func, PCI_MAXG_OFF));
	return 0;
}

int 
pci_get_devs(pci_class_t class, bdfo_t *ret, int *count) {
	uint16_t bus = 0, dev = 0, func = 0, temp = 0;
	*count = 0;

	FOREACH_PCI_DEV(bus, dev, func, temp) {
		uint16_t devid = pci_read16(PCI_BDFO(bus, dev, func, PCI_DEVID_OFF));
		if(devid == (uint16_t)0xFFFF)
			continue;
		bdfo_t match = PCI_BDFO(bus, dev, func, PCI_CLASS_OFF);
		pci_class_t read_class = pci_read8(match);
		if (read_class == class) {
			ret[*count] = match;
			*count = *count + 1;
		}
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
			printf("\t%04x %04x\n\t%02x %02x %02x %02x",
			pci_read16(PCI_BDFO(bus,dev,func, 0)), 
//			pci_read8(PCI_BDFO(bus,dev,func, 1)), 
			pci_read16(PCI_BDFO(bus,dev,func, 2)), 
//			pci_read8(PCI_BDFO(bus,dev,func, 3)), 
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
