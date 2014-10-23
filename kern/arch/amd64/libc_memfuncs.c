void memset(void *s, int c, uint64_t n)
{
	KWARN_NOW(c & ~0xff);
	c &= 0xff;
	const uint64_t qwords = n >> 3;
	const uint64_t bytes = n & 0x7;
	uint64_t byte = 0x0101010101010101;
	byte *= c;

	__asm__ volatile("rep stosq"
					 :
					 : "rax" (byte), "rcx" (qwords), "rdi" (s)
					 : "rcx", "rdi", "memory");
	if (!bytes)
		return;

	s += qwords;

	__asm__ volatile("rep stosb"
					 :
					 : "rax" (c), "rcx" (bytes), "rdi" (s)
					 : "rcx", "rdi", "memory");
}
