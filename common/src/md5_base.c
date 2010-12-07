// Algorithm from http://en.wikipedia.org/wiki/MD5

#include <stdint.h>
#include <md5.h>

// Rotate values copied directly from wiki
const static uint8_t r[64] = {
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,
};

// This was calculated based on if 0'd section below
const static uint32_t k[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

const static uint8_t md5_pad[MD5_BLOCK_SIZE] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void
init_md5_ctx(struct md5_context *ctx, uint8_t *data, uint64_t size) {
	ctx->h0 = 0x67452301;
	ctx->h1 = 0xEFCDAB89;
	ctx->h2 = 0x98BADCFE;
	ctx->h3 = 0x10325476;
	ctx->curptr = (void *)data;
	ctx->size = size;

	uint64_t bytes_to_write;
	int offset = size % MD5_BLOCK_SIZE;
	if (offset < MD5_PRE_PAD_BLOCK_SIZE) {
		bytes_to_write = MD5_PRE_PAD_BLOCK_SIZE - offset;
	} else {
		bytes_to_write = (MD5_PRE_PAD_BLOCK_SIZE + MD5_BLOCK_SIZE) - offset;
	}
	ctx->padded_size = size + bytes_to_write + 8;
}

static uint32_t 
leftrotate(uint32_t x, uint32_t y) {
	return (x << y) | (x >> (32UL - y));
}

int
pad_block(struct md5_context *ctx) {
	int ret = 0;
	int offset = ctx->size % MD5_BLOCK_SIZE;
	int bytes_to_write;
	if (offset < MD5_PRE_PAD_BLOCK_SIZE) {
		bytes_to_write = MD5_PRE_PAD_BLOCK_SIZE - offset;
	} else {
		bytes_to_write = (MD5_PRE_PAD_BLOCK_SIZE + MD5_BLOCK_SIZE) - offset;
		ret = bytes_to_write;
	}

	memcpy(ctx->curptr + offset, md5_pad, bytes_to_write);
	*(uint64_t *)(ctx->curptr + offset + bytes_to_write) = (ctx->size * 8);
	return ret;
}

void
md5_hash_block(struct md5_context *ctx) {
	uint32_t *words = ctx->curptr;
	uint32_t a = ctx->h0;
	uint32_t b = ctx->h1;
	uint32_t c = ctx->h2;
	uint32_t d = ctx->h3;
	uint32_t f, g, temp;
	int i;
	for(i = 0; i < 64; i++) {
		if (i >= 0 && i <=15) {
			f = (b & c) | ((~b) & d);
			g = i;
		} else if (i >= 16 && i <= 31) {
			f = (d & b) | ((~d) & c);
			g = (5 * i + 1) % 16;
		} else if (i >= 32 && i <= 47) {
			f = b ^ c ^ d;
			g = (3 * i + 5 ) % 16;
		} else if (i >= 48 && i <= 63) {
			f = c ^ (b | (~d));
			g = (7 * i) % 16;
		}
		temp = d;
		d = c;
		c = b;
		uint32_t test = (a + f + k[i] + words[g]);
		b = b + leftrotate(test, r[i]);
		a = temp;
	}

	ctx->h0 = ctx->h0 + a;
	ctx->h1 = ctx->h1 + b;
	ctx->h2 = ctx->h2 + c;
	ctx->h3 = ctx->h3 + d;
	ctx->hashed_bytes += MD5_BLOCK_SIZE;
}

void
display_md5hash(struct md5_context *ctx) {
	int i;
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&ctx->h0)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&ctx->h1)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&ctx->h2)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&ctx->h3)[i]);
	}
}


