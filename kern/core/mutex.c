#include <thread.h> // this_thread
#include <noarch.h> // this_thread
#include <arch/atomic.h>
#include <mutex.h>

void 
mutex_acquire(struct mutex *mtx) {
	while(!atomic_cmpxchg_64(&mtx->mutex_owner, 0, this_thread()->tid)) {
	
	}
}

void 
mutex_release(struct mutex *mtx) {
	KASSERT((atomic_cmpxchg_64(&mtx->mutex_owner, this_thread()->tid, 0)) == 1, (""));
}
