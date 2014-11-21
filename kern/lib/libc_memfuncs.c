
void *__hbuiltin_memcpy(void *dest, const void *src, uint64_t n);
void __hbuiltin_memset(void *s, char c, uint64_t n);
void __hbuiltin_bzero(void *s, uint64_t n);
void __hbuiltin_bcopy(const void *src, void *dest, uint64_t n);

void memcpy(void *dest, const void *src, uint64_t n) __attribute__((weak, alias ("__hbuiltin_memcpy")));
void arch_memset(void *s, char c, uint64_t n) __attribute__((weak, alias ("__hbuiltin_memset")));
void bzero(void *s, uint64_t n) __attribute__((weak, alias ("__hbuiltin_bzero")));
void bcopy(const void *src, void *dest, uint64_t n) __attribute__((weak, alias ("__hbuiltin_bcopy")));

/* 
 * Copy n bytes from src to dest, returns dest. Does nothing is 
 * src and dest overlap.
 */
void *
__hbuiltin_memcpy(void *dest, const void *src, uint64_t n) {
	void *ret = dest;
	if (n == 0)
		return dest;

	// check if memory areas overlap, if so do nothing
	KASSERT(src != dest, ("src == dest, you probably didn't mean for this to happen\n"));
	if (src == dest) {
		return dest;
	}

	if (src > dest) {
		KASSERT(((dest + n) <= src), ("(dest + n) > src, you probably didn't mean for this to happen\n"));
		if ((dest + n) > src) {
			return dest;
		}
	} else {
		KASSERT(((src + n) <= dest), ("(src + n) > dest, you probably didn't mean for this to happen\n"));
		if ((src + n) > dest) {
			return dest;
		}
	}

	while(n > 8) {
		*(uint64_t *)dest = *(uint64_t *)src;
		dest += 8;
		src += 8;
		n -= 8;
	}

	while(n--) {
		*(uint8_t *)dest = *(uint8_t *)src;
		dest++;
		src++;
	}
	
	return ret;
}

void
__hbuiltin_memset(void *s, char c, uint64_t n) {
	while(n--) {
		*(uint8_t *)s = (uint8_t)c;
	}
}

void *
memset(void *s, int c, uint64_t n)
{
	arch_memset(s, c, n);

	return s;
}

void 
__hbuiltin_bzero(void *s, uint64_t n) {
	memset(s, 0, n);
}

/* TODO implement this, bcopy is tricky */
void 
__hbuiltin_bcopy(const void *src, void *dest, uint64_t n) {
	memcpy(dest, src, n);
}
