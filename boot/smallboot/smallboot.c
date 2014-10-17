#include "legacy_int.h"
#include "loader.h"
#include "bios.h"
#include "small_libc.h"
#include "multiboot.h"
#define __ELF_WORD_SIZE 32
#include "elf.h"

#define SECTOR_SIZE 512
void
main(int size, unsigned char device, struct partition *partition_entry) __attribute__((noreturn));

static void load_kernel(const char *kern_name, char *args);

extern uint32_t *_realbstart, *_realbend;

struct e820_entry e820_map[MAX_E820_ENTRIES];
int our_e820_index;

struct multiboot_mmap_entry mboot_mmap[MAX_E820_ENTRIES];

static int total_e820_count;
unsigned char bss_sectors[SECTOR_SIZE * 127];
uint8_t bios_device;
int (*legacy_read_sector)(void *addr, unsigned char count, unsigned int lba[2], unsigned char bios_device);

/* machine independent read sector is the function that's passed to the
 * FS specific code for it to load files. The idea is the FS specific code
 * need not know much about the load medium. We pass a sector size to the
 * FS code, but that's it
 */
unsigned int
mi_read_sector(const void *addr, unsigned int count, unsigned int start[2]) {
	ASSERT((unsigned int)addr >= 0x100000);
	const int sector_size = SECTOR_SIZE;
	//
	// assume we can read at most 127 sectors at a time because of legacy BIOS
	int num_loops = count / 127;
	int read_amt = count % 127;
	do {
		// TODO: should we just switch to our legacy_int()???
		//int ret = legacy_read_sector(bss_sectors, read_amt, start, bios_device);
		struct dap {
			unsigned char size;
			unsigned char sbz;
			unsigned char count;
			unsigned char sbz2;
			unsigned int addr;
			unsigned int lba[2];
		} __attribute__((packed));
		struct dap dap;
		dap.size = sizeof(struct dap);
		dap.sbz = 0;
		dap.count = read_amt;
		dap.sbz2 = 0;
		dap.addr = (unsigned int) bss_sectors;
		dap.lba[0] = start[0];
		dap.lba[1] = start[1];
		struct legacy_regs_32 regs;
		regs.eax = 0x4200;
		regs.edx = bios_device;
		regs.esi = (uint32_t)&dap;
		int ret = legacy_int(0x13, &regs);
		ASSERT(((ret & 1) != 1) && ((regs.eax & 0xFF) == 0));

		count -= read_amt;
		memcpy((void *)addr, bss_sectors, read_amt * sector_size);
		addr += read_amt * sector_size;
		start[0] += read_amt;
		if(start[0] < read_amt) //overflowed
			start[1]++;
		read_amt = 127;
	} while(num_loops--);

	return count;
}

static void
build_memory_map()
{
	int e820_count = MAX_E820_ENTRIES;
	int return_flags = 0;
	struct e820_entry *entry = &e820_map[0];
	char smap[4] = {'P', 'A', 'M', 'S'}; //"SMAP"
	struct legacy_regs_32 regs = {
		.eax = 0xe820,
		.ebx = 0,
		.ecx = sizeof(struct e820_entry),
		.edi = (unsigned int)entry
	};
	memcpy(&regs.edx, smap, sizeof(smap));

	memset(e820_map, 0, sizeof(e820_map));
	do {
		return_flags = legacy_int(0x15, &regs);
		regs.eax = 0xe820;
		memcpy(&regs.edx, smap, sizeof(smap));
		#ifdef E820_DEBUG
		struct e820_entry *temp = (struct e820_entry *)regs.edi;
		printf("E820: %x%x %x%x %x\n",
			   temp->addr_high, temp->addr_low, temp->length_high, temp->length_low, temp->type);
		#endif
		regs.edi += sizeof(struct e820_entry);
	} while(regs.ebx != 0 && !(return_flags & 1) && --e820_count && ++total_e820_count);

	/*
	 * Example: BOCHS with 32 MB memory
	 * E820: 0000000000000000 000000000009f000 00000001
	 * E820: 000000000009f000 0000000000001000 00000002
	 * E820: 00000000000e8000 0000000000018000 00000002
	 * E820: 0000000000100000 0000000001ef0000 00000001
	 * E820: 0000000001ff0000 0000000000010000 00000003
	 * E820: 00000000fffc0000 0000000000040000 00000002
	 */
}

static struct e820_entry *
get_suitable_e820_entry() {
	int i;
	struct e820_entry *which = 0;
	for (i = 0; i < MAX_E820_ENTRIES; i++) {
		// find a suitably large section to give to the loader
		if (e820_map[i].type != E820_FREE)
			continue;

		//unsigned int basel =  e820_map[i].addr_low;
		//unsigned int baseh =  e820_map[i].addr_high;
		unsigned int lengthl = e820_map[i].length_low;
		unsigned int lengthh = e820_map[i].length_high;

		if (which == 0) {
			which = &e820_map[i];
		} else {
			if (!which->addr_high && which->addr_low < 0x100000) {
				which = &e820_map[i];
				continue;
			}
			if (lengthh > which->length_high ||
				((lengthh == which->length_high) && (lengthl > which->length_low))){
				which = &e820_map[i];
				continue;
			}
		}
	}
	return which;
}
static void
clear_bss() {
	// TODO: endian fix?
	unsigned int bss_end = (unsigned int)&_realbend;
	unsigned int bss_start = (unsigned int)&_realbstart;
	unsigned int bss_size = bss_end - bss_start;
	/* clear out bss */
	memset((void *)&_realbstart, 0, bss_size);
}

static uint16_t
check_extensions(int drive) {
	struct legacy_regs_32 regs = {
		.eax = 0x4100,
		.ebx = 0x55aa,
		.edx = (uint8_t) drive,
	};
	uint32_t return_flags = legacy_int(0x13, &regs);
	if (return_flags & 1) {
		printf("No BIOS extensions supported\n");
	} else {
		if((regs.eax & 0xFF00) == 0x3000 &&
			(regs.ebx & 0xFFFF) == 0xAA55) {
				return regs.ecx;
			} else {
				printf("weird %x %x\n", regs.eax, regs.ebx);
			}
	}
	return 0;
}

static uint16_t
get_sector_size(int drive) {
#ifdef BOCHS_INT13_WORKAROUND
	int8_t buf[26];
	buf[0] = 26;
#else
	uint8_t buf[100];
	buf[0] = 100;
#endif
	struct legacy_regs_32 regs = {
		.eax = 0x4800,
		.edx = (uint8_t) drive,
		.esi = (uint32_t)buf
	};
	legacy_int(0x13, &regs);
	if (buf[2] & 0x2) {
		return ((uint16_t)buf[25]) << 8 | buf[24];
	}

	return 0;
}
struct partition partition_table[4];

#define HOBOLOAD_VER 0
#define HOBOLOAD_DRIVE 1
#define HOBOLOAD_PARTITION 2
#define HOBOLOAD_KERNEL_NAME 3
#define HOBOLOAD_ARGS 4
static char *hload_info[HOBOLOAD_ARGS+1];
void main(int size, unsigned char device, struct partition *partition_entry)
{
	clear_bss();
	int i;

	/* Make a big assumption here, and get the partition table */
	memcpy(partition_table, (void *)(0x7a00 + 0x1be), sizeof(struct partition) * 4);

	bios_device = device;
	ASSERT(check_extensions(device) & INT13_EXT_FIXED_DISK_SUPPORT);
	ASSERT(get_sector_size(device) == SECTOR_SIZE);
	puts("Welcome to HOBos\n");

	build_memory_map();
	const struct e820_entry *which = get_suitable_e820_entry();
	ASSERT(which != 0);

	/*  turn the thing into a mb# */
	unsigned int start_mb;
	unsigned int num_mb;
	start_mb = (which->addr_low >> 20);
	start_mb |= ((which->addr_high & 0xFFFFF) << 12);
	if (which->addr_low & 0x000FFFFF) {
		start_mb++; // must round up if it wasn't aligned
	}
	num_mb = (which->length_low >> 20);
	num_mb |= ((which->length_high & 0xFFFFF) << 12);

	if (num_mb & 0x80000000) {
		num_mb = 0x7FFFFFFF;
	}

	if (start_mb == 0 && which->addr_high) {
		// we only have 32 bits... what do we do, this is fixable?
		ASSERT(0);
	} else if (start_mb == 0 && num_mb != 0) {
		start_mb += 0x1;
		num_mb -= 1;
	}

	ASSERT(start_mb >= 0x1);
	ASSERT(num_mb >= 1); // how many MBs is sufficient

	struct load_params params = {
		.read_sector = mi_read_sector,
		.sector_size = SECTOR_SIZE,
		.start_mb = start_mb + 2,
		.num_mb = num_mb,
		.private_data = e820_map,
		.drive = device,
		.partitions = partition_entry
	};

	initialize_block_loader(&params);

	void *addr = 0;

	unsigned int num_chars = load_file("hload", &addr);
	ASSERT(num_chars > 0);

	i = HOBOLOAD_VER;
	//hload_info[i++] = addr;
	char *args = addr;
	char *prev = addr;
	do {
		args = strnchr(args, ' ', num_chars);
		if (args != 0) {
			*args = 0;
			hload_info[i++] = prev;
			args++;
			num_chars -= (args - prev);
			prev = args;
		} else {
			break;
		}
	} while (i < HOBOLOAD_ARGS);

	char *kernel = hload_info[HOBOLOAD_KERNEL_NAME];
	ASSERT(strlen(kernel) > 0);
	printf("loading \"%s\"\n", kernel);
	load_kernel(kernel, args);

	while(1);
}

static unsigned int
calc_lower_mem() {
	unsigned int ret = 0;
	/*  We assume the e820 table is sorted */
	struct e820_entry *entry = &e820_map[0];
	while(entry->type != 0) {
		if (entry->addr_high)
			continue;
		if (entry->addr_low < 0x100000 && entry->type == E820_FREE) {
			ASSERT(entry->length_high == 0);
			ret += entry->length_low;
		}
		entry++;
	}

	return ret;
}

static unsigned int
calc_upper_mem() {
	unsigned int ret = 0;
	/*  We assume the e820 table is sorted */
	struct e820_entry *entry = &e820_map[0];
	while(entry->type != 0) {
		if (entry->addr_low >= 0x100000 && entry->type == E820_FREE) {
			ASSERT(entry->length_high == 0);
			ret += entry->length_low;
			break;
		}
		entry++;
	}

	return ret;
}
#include "multiboot.h"

static struct multiboot_info
create_mulitboot_info_struct(struct multiboot_elf_section_header_table *table) {
	int i = 0;
	struct multiboot_info multiboot_info;

	multiboot_info.flags = 1;
	multiboot_info.mem_lower = calc_lower_mem();
	multiboot_info.mem_upper = calc_upper_mem();
	#ifdef BOOT_DEVICE
	multiboot_info.flags |= 1 << 1;
	multiboot_info.boot_device = bios_device;
	#error need to get the partition table
	#endif
	multiboot_info.flags |= MULTIBOOT_INFO_CMDLINE;
	multiboot_info.cmdline = (multiboot_uint32_t)hload_info[HOBOLOAD_ARGS];
	multiboot_info.flags |= MULTIBOOT_INFO_MODS;
	multiboot_info.mods_count = 0;
	multiboot_info.mods_addr = 0;
	multiboot_info.flags |= MULTIBOOT_INFO_ELF_SHDR;
	multiboot_info.u.elf_sec = *table;
	multiboot_info.flags |= MULTIBOOT_INFO_MEM_MAP;
	for(i = 0; i < total_e820_count; i++) {
		mboot_mmap[i].size = sizeof(struct multiboot_mmap_entry);
		mboot_mmap[i].addr = e820_map[i].addr_high;
		mboot_mmap[i].addr <<= 32;
		mboot_mmap[i].addr |= e820_map[i].addr_low;
		mboot_mmap[i].len = e820_map[i].length_high;
		mboot_mmap[i].len <<= 32;
		mboot_mmap[i].len |= e820_map[i].length_low;
		mboot_mmap[i].type = e820_map[i].type;
	}
	multiboot_info.mmap_length = total_e820_count * sizeof(struct multiboot_mmap_entry);
	multiboot_info.mmap_addr = (multiboot_uint32_t)mboot_mmap;
	return multiboot_info;
}

void *elf_load_helper(const void *addr, unsigned int loaded_len,
					  struct multiboot_elf_section_header_table *ret)
{
extern void *elf_load64(const void *, void *, int, int, struct multiboot_elf_section_header_table *);
extern void *elf_load32(const void *, void *, int, int, struct multiboot_elf_section_header_table *);

	const char elf_magic[4] = { 0x7f, 'E', 'L', 'F' };
	char *e_ident = (char *)addr;
	const void *ehdr = addr;
	ASSERT(strncmp(e_ident, elf_magic, 4) == 0);
	ASSERT(e_ident[EI_DATA] == ELFDATA2LSB);

	switch (e_ident[EI_CLASS]) {
	case ELFCLASS64:
		return elf_load64(addr,
						  (void *)addr + ((Elf64_Ehdr *)ehdr)->e_shoff,
						  ((Elf64_Ehdr *)ehdr)->e_shnum,
						  ((Elf64_Ehdr *)ehdr)->e_shstrndx,
						  ret);
		break;
	case ELFCLASS32:
		return elf_load32(addr,
						  (void *)addr + ((Elf32_Ehdr *)ehdr)->e_shoff,
						  ((Elf32_Ehdr *)ehdr)->e_shnum,
						  ((Elf32_Ehdr *)ehdr)->e_shstrndx,
						  ret);
		break;
	default:
		ASSERT(0);
	}
}

static int
load_multiboot_kernel(const char *kern_name, char *args) {
	void *addr = 0;
	int found = 0;

	/*  try multiboot first */
	unsigned int nbytes = load_file_bytes(kern_name, &addr, MULTIBOOT_SEARCH);
	if (nbytes < 8 ) {
		printf("File not found or too small\n");
		return 1;
	}

	do {
		struct multiboot_header *mboot_hdr = (struct multiboot_header *)addr;

		if (
			(mboot_hdr->magic == MULTIBOOT_HEADER_MAGIC) &&
			(~(mboot_hdr->magic + mboot_hdr->flags)) == (mboot_hdr->checksum - 1)) {
			found++;
			break;
		}

		addr += 4;
	} while (nbytes -= 4) ;

	if (!found)
		return 1;

	/* Load the rest of the file so that we can do the elf stuff */
	addr = 0;
	nbytes = load_file(kern_name, &addr);

	struct multiboot_elf_section_header_table table;
	void (*kernel_entry)(void) = (void *)elf_load_helper(addr, nbytes, &table);
	if (kernel_entry == 0) {
		return -1;
	}

	struct multiboot_info mboot_info = create_mulitboot_info_struct(&table);
	uint32_t current_esp  = 0;
	__asm__ volatile("movl	%%esp, %0;" : "=r" (current_esp));
	ASSERT(current_esp > 0x1000 && current_esp < 0x9f000);
	__asm__ volatile(
		"call *%1"
		:
		: "a" (MULTIBOOT_BOOTLOADER_MAGIC), "g" (kernel_entry), "b" (&mboot_info));
	return 0;
}

static void
load_kernel(const char *kern_name, char *args) {

	/*  try multiboot first */
	if (load_multiboot_kernel(kern_name, args) == 0) {
		return;
	}

	printf("Not a multiboot kernel\n");
	ASSERT(0);
}
