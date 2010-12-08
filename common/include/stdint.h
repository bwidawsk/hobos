#ifndef _STDINT_H_
#define _STDINT_H_

typedef int int8_t __attribute__ ((mode (QI)));
typedef unsigned int uint8_t __attribute__ ((mode (QI)));
typedef int int16_t __attribute__ ((mode (HI)));
typedef unsigned int uint16_t __attribute__ ((mode (HI)));
typedef int int32_t __attribute__ ((mode (SI)));
typedef unsigned int uint32_t __attribute__ ((mode (SI)));
typedef int int64_t __attribute__ ((mode (DI)));
typedef unsigned int uint64_t __attribute__ ((mode (DI)));

typedef uint64_t uintmax_t;
#define addr_t uint##ARCH_WIDTH_BITS##_t

#endif
