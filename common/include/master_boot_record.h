#ifndef _MASTER_BOOT_RECORD_H_
#define _MASTER_BOOT_RECORD_H_

struct partition_type {
	unsigned char type;
	const char desc[64];
};

#define LINUX_PARTITION_TYPE 0x82

struct partition_type  part_types[] = {
	{0x00, "Empty partition"},
	{0x01, "FAT 12"},
	{0x02, "XENIX root"},
	{0x03, "XENIX usr"},
	{0x04, "FAT 16 with a size < 32 MiB"},
	{0x05, "Extended partition"},
	{0x06, "FAT 16 with size \u226532 MiB"},
	{0x07, "HPFS or NTFS"},
	{0x08, "AIX"},
	{0x09, "AIX bootable"},
	{0x0A, "OS/2 Boot Manager"},
	{0x0B, "Windows 95 FAT 32"},
	{0x0C, "Windows 95 FAT 32 with LBA"},
	{0x0E, "Windows 95 FAT 16 with LBA"},
	{0x0F, "Extended partition with LBA"},
	{0x10, "OPUS"},
	{0x11, "Hidden FAT 12"},
	{0x12, "Compaq diagnostics partition"},
	{0x14, "Hidden FAT 16"},
	{0x17, "Hidden HPFS or hidden NTFS"},
	{0x1B, "Hidden FAT 32"},
	{0x1C, "Hidden FAT 32 with LBA"},
	{0x1D, "Hidden FAT 16 with LBA"},
	{0x78, "Vos XOSL bootloader filesystem"},
	{0x82, "Linux swap space or Solaris (operating system)"},
	{0x83, "Any native Linux file system"},
	{0x85, "Linux extended"},
	{0x86, "Legacy FT FAT 16"},
	{0x87, "Legacy FT NTFS"},
	{0x88, "Linux plaintext"},
	{0x89, "Linux LVM"},
	{0x8B, "Legacy FT FAT 32"},
	{0x8C, "Legacy FT FAT 32 with LBA"},
	{0x8E, "Linux LVM"},
	{0xA5, "BSD slice"},
	{0xDA, "Raw data (no File System)"},
	{0xDF, "Unlimited BootIt"},
	{0xEB, "BFS (BeOS or Haiku)"},
	{0xEF, "EFI"},
	{0xFB, "VMware VMFS"},
	{0xFC, "VMware VMKCORE"},
	{0xFD, "Linux RAID auto"},
	{0xFF, "Invalid"}
};

static inline char *
lookup_partition_type(unsigned char type) {
	char *ret = NULL;
	struct partition_type *part;
	int i;

	for(i = 0; ;i++) {
		part = &part_types[i];
		if (part->type == type)
			ret = (char *)part->desc;
	}

	return ret;
}

struct primary_partition {
	uint8_t status;
	uint8_t chs_first[3];
	uint8_t type;
	uint8_t chs_last[3];
	uint32_t lba_first;
	uint32_t count;
}__attribute__((__packed__));

struct master_boot_record {
	uint8_t code[440];
	uint32_t signature;
	uint16_t sbz;
	struct primary_partition partitions[4];
	uint16_t sig;
}__attribute__((__packed__));

#endif
