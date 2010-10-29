struct mutex {
	char *name;
	int flags; // TBD
	void *owner;
};

#define MUTEX_DECLARE(mtxname) \
		static struct mutex mtxname = { \
			.name = #mtxname, \
		} \

struct mutex *mutex_init(char *name);
void mutex_destroy(struct mutex *mtx);
void mutex_acquire(struct mutex *mtx);
void mutex_release(struct mutex *mtx);