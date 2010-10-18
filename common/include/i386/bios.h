#ifndef _HOBOS_BIOS_H_
#define _HOBOS_BIOS_H_

#include "stdint.h"

struct partition  {
	uint8_t status;
	uint8_t s_head;
	uint8_t s_sec_cyl;
	uint8_t s_cyl;
	uint8_t type;
	uint8_t e_head;
	uint8_t e_sec_cyl;
	uint8_t e_cyl;
	uint32_t lba;
	uint32_t count;
} __attribute__((packed));

struct e820_entry {
	uint32_t addr_low;
	uint32_t addr_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t type;
} __attribute__((packed));

enum {
	E820_FREE=1,
	E820_RSVD=2,
	E820_ACPI_RECLAIM=3,
	E820_ACPI_SAVE=4
};

struct dap {
	uint8_t size;
	uint8_t sbz;
	uint8_t count;
	uint8_t sbz2;
	uint32_t addr;
	uint32_t lba[2];
} __attribute__((packed));

#define INT13_EXT_FIXED_DISK_SUPPORT (1 << 0)
#define INT13_EXT_DEVICE_LOCKING (1 << 1)
#define INT13_EXT_EDD (1 << 2)
#define INT13_64_BIT (1 << 3)

struct extended_dap {
	uint8_t size;
	uint8_t sbz;
	uint8_t count;
	uint8_t sbz2;
	uint32_t addr;
	uint64_t long_addr;
	uint64_t long_count;
	uint64_t long_size;
};



#endif