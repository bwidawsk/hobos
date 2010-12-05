#ifndef _MD5_H_
#define _MD5_H_

#define MD5_BLOCK_SIZE (512 / 8) // 512 bit blocks
#define MD5_PRE_PAD_BLOCK_SIZE (448 / 8) // 512 bit blocks

struct md5_context {
	uint64_t size;
	void *curptr;
	uint32_t h0;
	uint32_t h1;
	uint32_t h2;
	uint32_t h3;
};

void init_md5_ctx(struct md5_context *ctx, uint8_t *data, uint64_t size);
uint64_t pad_md5(uint8_t **orig_data, uint64_t size);
void md5_hash_block(struct md5_context *ctx);

#endif
