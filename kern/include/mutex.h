struct mutex {
	char *mutex_name;
	int mutex_flags; // TBD
	void *mutex_owner;
};

#define MUTEX_DECLARE(mtxname) \
		static struct mutex mtxname = { \
			.mutex_name = #mtxname, \
		} \

#define MUTEX_INIT(mtx, name) \
	mtx = malloc(sizeof(struct mutex)); \
	mtx->mutex_name = name

struct mutex *mutex_init(char *name);
void mutex_destroy(struct mutex *mtx);
void mutex_acquire(struct mutex *mtx);
void mutex_release(struct mutex *mtx);
