#include <mm/malloc.h>
#include <device.h>

static int last_dev=0;
struct device *all_devices[CONFIG_MAX_SYSTEM_DEVICES];

int
device_register(struct device *dev, int type) {
	dev->type = type;
	all_devices[last_dev] = dev;
	last_dev++;
	return 0;
}

/*
 * Use quicksort?
 */
struct device *
device_get(int type, int order) {
	struct device *temp_devices[CONFIG_MAX_SYSTEM_DEVICES];
	int i, count=0;

	for (i = 0; i < CONFIG_MAX_SYSTEM_DEVICES; i++) {
		struct device *temp = all_devices[i];
		if (temp == NULL)
			continue;

		if (type == -1 || temp->type == type) {
			temp_devices[i] = all_devices[i];
			count++;
		}
	}

	if (order >= count)
		return NULL;
	else
		return temp_devices[order];
}
