// Algorithm from http://en.wikipedia.org/wiki/MD5

#include <stdint.h>
#include <md5.h>
#ifndef KERNEL
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#endif

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
int main(int argc, char *argv[]) {
	if (argc > 1) {
		int fd;
		struct stat sb;
		uint64_t size_to_md5 = 0;
		fd = open(argv[1], O_RDONLY);
		if (fd == -1) {
			perror("open");
			exit(-1);
		}
		if (fstat(fd, &sb) == -1) {
			perror("fstat");
			exit(-1);
		}
		size_t file_size = sb.st_size;

		void *addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

		if (addr == MAP_FAILED) {
			perror("mmap");
			exit(-1);
		}

		uint8_t *buf = (uint8_t *)malloc(file_size + (MD5_FILE_PADDING));
		memcpy(buf, addr, file_size);
		munmap(addr, file_size);

		struct md5_context ctx;
		init_md5_ctx(&ctx, buf, file_size);

		//pad_md5(buf, file_size);

		uint64_t num_chunks = ctx.size / MD5_BLOCK_SIZE;
		if (((ctx.size % MD5_BLOCK_SIZE) == 0) && ctx.size != 0)
			num_chunks++;
		// go until the last block which may need to be padded
		for (;num_chunks > 1 ; num_chunks--) {
			md5_hash_block(&ctx);
			ctx.curptr += MD5_BLOCK_SIZE;
		}
		int need_another_block = pad_block(&ctx);
		md5_hash_block(&ctx);
		if (need_another_block) {
			ctx.curptr += MD5_BLOCK_SIZE;
			md5_hash_block(&ctx);
		}
		display_md5hash(&ctx);
		printf("\t%s\n", argv[1]);
		free(buf);
		close(fd);
	}
	return 0;
}

#if 0
#include <math.h>
void
create_k_values() {
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
}


#endif
