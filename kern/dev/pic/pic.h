#ifndef __PIC_H__
#define __PIC_H__

struct pic_dev {
	int (*init)(int);
	void (*mask)(int);
	void (*unmask)(int);
	void (*eoi)(int);
};

#endif
