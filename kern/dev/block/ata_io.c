#include <arch/asm.h>
#include "ata.h"

#define ATA_IO_DEBUG 0

uint8_t
ata_read8_io(struct ata_channel *ata, uint8_t which) {
	uint16_t which_base = ata->ata_cmd_base & 0xffff;
	if (which > ATA_CMD_MAX) {
		which_base = ata->ata_ctrl_base & 0xffff;
		which -= ATA_CMD_MAX;
	}
	#if (ATA_IO_DEBUG > 0)
		uint8_t ret = inb(which_base + which);
		printf("%s: %x (%x)\n", __FUNCTION__, which_base + which, ret);
		return ret;
	#else
		return inb(which_base + which);
	#endif
}

uint16_t
ata_read16_io(struct ata_channel *ata, uint8_t which) {
	uint16_t which_base = ata->ata_cmd_base & 0xffff;
	if (which > ATA_CMD_MAX) {
		which_base = ata->ata_ctrl_base & 0xffff;
		which -= ATA_CMD_MAX;
	}
	#if (ATA_IO_DEBUG > 0)
		uint16_t ret = inb(which_base + which);
		printf("%s: %x (%x)\n", __FUNCTION__, which_base + which, ret);
		return ret;
	#else
		return inw(which_base + which);
	#endif
}

void
ata_write8_io(struct ata_channel *ata, uint8_t which, uint8_t data) {
	uint16_t which_base = ata->ata_cmd_base & 0xffff;
	if (which > ATA_CMD_MAX) {
		which_base = ata->ata_ctrl_base & 0xffff;
		which -= ATA_CMD_MAX;
	}
	#if (ATA_IO_DEBUG > 0)
		printf("%s: %x (%x)\n", __FUNCTION__, which_base + which, data);
	#endif
	return outb(data, which_base + which);
}

void
ata_write16_io(struct ata_channel *ata, uint8_t which, uint16_t data) {
	uint16_t which_base = ata->ata_cmd_base & 0xffff;
	if (which > ATA_CMD_MAX) {
		which_base = ata->ata_ctrl_base & 0xffff;
		which -= ATA_CMD_MAX;
	}
	#if (ATA_IO_DEBUG > 0)
		printf("%s: %x (%x)\n", __FUNCTION__, which_base + which, data);
	#endif
	return outw(data, which_base + which);
}
