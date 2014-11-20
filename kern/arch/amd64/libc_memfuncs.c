void arch_memset(void *s, int c, uint64_t n)
{
	KWARN_NOW(c & ~0xff);
	c &= 0xff;
	uint64_t qwords = n >> 3;
	uint64_t bytes = n & 0x7;
	uint64_t byte = 0x0101010101010101;
	uint64_t cx, di;
	byte *= c;

	__asm__ volatile("rep stosq"
					 : "=&c" (cx), "=&D" (di)
					 : "a" (byte), "0" (qwords), "1" (s)
					 : "memory");
	if (!bytes)
		return;

	s += qwords;

	__asm__ volatile("rep stosb"
					 : "=&c" (cx), "=&D" (di)
					 : "a" ((uint8_t)c), "0" (bytes), "1" (s)
					 : "memory");
}
