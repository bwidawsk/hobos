#ifndef __DEVICE_H__
#define __DEVICE_H__

enum {
	PHYSICAL_MEMORY_DEVICE,
	BLOCK_DEVICE
};

struct device {
	uint32_t type;
};

int device_register(struct device *dev, int type);
struct device *device_get(int type, int order);

#endif
