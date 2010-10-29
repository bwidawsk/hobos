#include <stdint.h>
#include "atomic_generic.h"
#include "mutex.h"

#define curthread 4

void 
mutex_acquire(struct mutex *mtx) {
	while(!atomic_cmpxchg_64(&mtx->owner, 0, curthread)) {
	
	}
}

void 
mutex_release(struct mutex *mtx) {
	KASSERT((atomic_cmpxchg_64(&mtx->owner, curthread, 0)) == 1, (""));
}