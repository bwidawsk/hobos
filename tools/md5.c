#include <stdint.h>

// Algorithm from http://en.wikipedia.org/wiki/MD5

// copied directly from wiki
const uint8_t r[64] = {
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,
};

// THis was calculated based on if 0'd section below
const uint32_t k[] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

#define MD5_BLOCK_SIZE (512 / 8) // 512 bit blocks

uint32_t leftrotate(uint32_t x, uint32_t y) {
	return (x << y) | (x >> (32UL - y));
}

void
pad_md5(uint8_t *data, uint64_t size) {
	int remainder = size % MD5_BLOCK_SIZE;
	if (size == 0)
		remainder = MD5_BLOCK_SIZE;

	if (remainder == 0)
		return;

	if(remainder <= 8) {
		printf("not sure how to handle this\n");
	}

	int first_bytes = MD5_BLOCK_SIZE - remainder;
	data[first_bytes++] = 0x80;
	remainder--;
	while(remainder > 8) {
		data[first_bytes++] = 0;
		remainder--;
	}

	*(uint64_t *)(&data[first_bytes]) = (size * 8);
}

void
md5_chunk(uint8_t *data, uint64_t num_chunks) {
	uint64_t chunk = 0;
	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;

	for(chunk = 0; chunk < num_chunks; chunk++) {
		uint32_t *words = (uint32_t *)(&data[chunk]);
		uint32_t a = h0;
		uint32_t b = h1;
		uint32_t c = h2;
		uint32_t d = h3;
		uint32_t f, g, temp;
		int i;
		for(i = 0; i < 64; i++) {
			//printf ("%x %x %x %x %x ", a, b, c, d, k[i]);
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
//			printf(" = %x\n", b);
			a = temp;
		}

		//printf ("\t\t%x %x %x %x\n", a, b, c, d);
		h0 = h0 + a;
		h1 = h1 + b;
		h2 = h2 + c;
		h3 = h3 + d;
	}
	int i;
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&h0)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&h1)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&h2)[i]);
	}
	for(i = 0; i < 4; i++) {
		printf("%02x", ((uint8_t*)&h3)[i]);
	}
	printf("\n");
}

void
dump_block(uint8_t *block) {
	int i;
	for(i = 0; i < MD5_BLOCK_SIZE; i+=8) {
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
		block[i + 0], 
		block[i + 1], 
		block[i + 2], 
		block[i + 3], 
		block[i + 4], 
		block[i + 5], 
		block[i + 6], 
		block[i + 7]);
	}

}
int main() {
	uint8_t foo[512];
	memset(foo, 0, 512);
	pad_md5(foo, 0);
//	dump_block(foo);
	md5_chunk(foo, 1);
#if 0
#include <math.h>
	int i;
	for(i = 0; i < 64; i+=8) {
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 1)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 2)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 3)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 4)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 5)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 6)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 7)) * pow(2, 32)));
		printf("0x%08x, ", (unsigned int)floor(fabs(sin(i + 8)) * pow(2, 32)));
		printf("\n");
	}
#endif
	return 0;
}
