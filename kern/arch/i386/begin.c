#include "../ia_common/gdt_defines.h"
#include "../ia_common/inlined_asm.h"
#include "../ia_common/ia_defines.h"


extern inline void lgdt(uint32_t);

void begin() __attribute__((noreturn));

struct segment_descriptor gdt[NUM_SEGMENTS] = {
	GDT_SEGMENTS
	};

struct gdt_descriptor gdtdesc = {
	.base = &gdt,
	.limit = (NUM_SEGMENTS * sizeof(struct segment_descriptor)) - 1
} ;

/* 
 * we expect we're in the first gb, and that we use 4MB pages 
 */
 // 1 GB / 4MB = 0x100 0x40000000 / 0x400000
 // this means we need 256 PDEs, each 32 bits
pde_4m_t early_page_tables[256];

/* Do stuff and set flags based on values read in CPU id. */
void
cpuid_setup() {

}

void
create_early_pages() {
	int i = 0;
	for(i = 0; i < 256; i++) {
		
	}
}

void 
begin() {
	/* 
	 * Because we loaded multiboot, we know we're in 32 bit protected mode but
	 * we don't trust the GDT, so load our own
	 */
	lgdt((uint32_t)&gdtdesc);
	
	cpuid_setup();
	while(1);
}
