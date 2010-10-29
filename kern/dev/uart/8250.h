#ifndef _8250_H_
#define _8250_H_

	#define INPUT_CLOCK_8250 1843200
	#define DIVISOR
	#define DIVISOR_TO_DLL(divisor) (((INPUT_CLOCK_8250 / 16) & 0xFF) / divisor)
	#define DIVISOR_TO_DLM(divisor) \
		((((INPUT_CLOCK_8250 / 16) & 0xFF00) >> 8)  / divisor)
	
#endif
	