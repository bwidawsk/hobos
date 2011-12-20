#define MAX_E820_ENTRIES 20

struct load_params {
	unsigned int (*read_sector)
		(const void *, unsigned int, unsigned int[2]); /* function to read a sector 
													from the boot medium */
	int sector_size; /* sector size of boot media */
	unsigned int start_mb; /* safe physical page to start */
	unsigned int num_mb; /* end of safe area  (by page)*/
	unsigned char drive; /* used in read_sector() */
	void *private_data;
	struct partition *partitions; /* list of partitions (machine dependent)*/
};

void initialize_block_loader(struct load_params *lparams);
unsigned int load_file_bytes(const char *name, void **addr, int nbytes);
#define load_file(name, addr) load_file_bytes(name, addr, -1)
