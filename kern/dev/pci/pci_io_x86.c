struct pci_accessor pci_io_x86 

uint32_t pci_read32(bdfo_t bdfo) {
	// write cf8 bdf0
	// write cf8 bit 31
	// return read cfc
}