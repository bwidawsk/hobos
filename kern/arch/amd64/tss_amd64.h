#ifndef __TSS_AMD64_H_
#define __TSS_AMD64_H_

struct tss_64 {
	uint64_t rsvd0:32;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t rsvd1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t rsvd2;
	uint64_t rsvd3:16;
	uint64_t iobase:16;
} __attribute__((packed));

#endif