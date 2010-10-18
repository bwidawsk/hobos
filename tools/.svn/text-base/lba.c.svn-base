#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>

int getopt(int argc, char * const argv[],
const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

#include <getopt.h>

// these are what I have set in bochs
#define HPC 16
#define SPT 63


int main(int argc, char *argv[]) {
	int opt;
	int heads_per_cyl = HPC;
	int sectors_per_track = SPT;
	int64_t lba = -1;
	char which;
	while ((opt = getopt(argc, argv, "h:s:l:d:")) != -1) {
	    switch (opt) {
		case 'h':
			heads_per_cyl = atoi(optarg);
			break;
		case 's':
			sectors_per_track = atoi(optarg);
			break;
		case 'l':
			lba = atoll(optarg);
			break;
		case 'd':
			which = *optarg;
			break;
		default: /* '?' */
			exit(-1);
		}
	}
	if (lba == -1) {
		return -1;
	}

	int cyl = lba / (heads_per_cyl * sectors_per_track);
	int temp = lba % (heads_per_cyl * sectors_per_track);
	int head = temp / sectors_per_track;
	int sec = temp % sectors_per_track + 1;

	if (which == 'c') 
		printf("%d\n", cyl);
	else if (which == 'h')
		printf("%d\n", head);
	else if (which == 's')
		printf("%d\n", sec);
	else {
		int chs1 = head;
		int chs2 = sec & 0x3f | ((cyl >> 2) & 0xc0);
		int chs3 = cyl & 0xFF;
		printf("%d %d %d\n", chs1, chs2, chs3);
	}
	return 0;
}
