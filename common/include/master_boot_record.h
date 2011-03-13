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
