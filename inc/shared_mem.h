#ifndef __SHARED_MEMORY
#define __SHARED_MEMORY

#include <pthread.h>

#define SHARED_MEM_FILE_NAME "/smartio_shm"
#define SHARED_MEM_PATH "/dev/shm"

/**
 * @brief this is a shared structure that can be shared between unrelated process of this executable. or any shared data that is needed in between two processes
 * 
 */
typedef struct{
	int last_power_selected;
	int count;
}stShared_data_t;

struct stMutLock {
	pthread_mutex_t mtx_lock;
	int pid;
};

struct mydata {
	
	struct stMutLock stAnalog_lock;
	struct stMutLock stModbus_lock;
	stShared_data_t st_shm_data;
};

extern struct stMutLock *pstLock;
extern struct mydata *stMy_data;


int shared_mem_init(char *);
void shared_mem_deinit(void);
void shared_mem_mutex_lock(struct stMutLock*);
void shared_mem_mutex_unlock(struct stMutLock*);
void system_init();
void system_Deinit();
#endif

