#ifndef _MD5_H_
#define _MD5_H_

#define MD5_BLOCK_SIZE (512 / 8) // 512 bit blocks
#define MD5_PRE_PAD_BLOCK_SIZE (448 / 8) // 512 bit blocks


// This is the amount of extra space the algorithm could right into at the end of
// the last block. In the worst case, a block is congruent to 56 bytes, so padding will
// end up writing in the remaining bytes, plus a full block, 64 + (64 - 56) = 72
#define MD5_FILE_PADDING (MD5_BLOCK_SIZE + (MD5_BLOCK_SIZE - MD5_PRE_PAD_BLOCK_SIZE))

struct md5_context {
	uint64_t size;
	uint64_t hashed_bytes;
	uint64_t padded_size;
	void *curptr;
	uint32_t h0;
	uint32_t h1;
	uint32_t h2;
	uint32_t h3;
};

// pseudocode for doing an md5 has is as follows:
// ctx = init_md5_ctx()
// while(nblks > 1)
//		md5_hash_block(ctx)
//		update ctx.curptr
// int ret=pad_block(ctx)
// md5_hash_block(ctx)
// if (ret)
//		update curptr
//		md5_hash_block(ctx)

// Initialize a context, data could be NULL for smart code which wishes to control
// curptr directly
void init_md5_ctx(struct md5_context *ctx, uint8_t *data, uint64_t size);

// Pad the last block, returns 1 if an extra block was needed to pad, returns 0 otherwise
int pad_block(struct md5_context *ctx);

// Hash the block at ctx.curptr
void md5_hash_block(struct md5_context *ctx);

#endif
