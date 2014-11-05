static inline int generic_popcount(uint64_t operand)
{
	/* Split the input into 8 elements of 8. Then use a lookup table to check 4 bits
	 * and a time. */
	static const int bit_lut[16] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 3
	};
	uint8_t arrays[8];
	int i, popcount = 0;

	memcpy(arrays, &operand, 8);
	for (i = 0; i < 8; i++) {
		/* First half of the byte */
		popcount += bit_lut[arrays[i] & 0xf];

		/* Second half of the byte */
		popcount += bit_lut[arrays[i] >> 4];
	}

	return popcount;
}
