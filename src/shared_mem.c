#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

struct mydata *stMy_data = NULL;
char mutex_lock_timeout = 0;

void recover(struct stMutLock *pstLock)
{
	CLOG("Performing Recovery by Pid : %d, Locked by: %d \n", getpid(), pstLock->pid);
	/* erase owner record  */
	pthread_mutex_consistent(&(pstLock->mtx_lock));
	/* reset shared data to valid state before unlocking mutex */
	pthread_mutex_unlock(&(pstLock->mtx_lock));	
	DLOG("Recovery completed by Pid : %d \n", getpid());
	pthread_mutex_lock(&(pstLock->mtx_lock));
}


void shared_mem_mutex_lock(struct stMutLock *pstLock){
	int rv = -1;
	mutex_lock_timeout = 0;
	alarm(config.mutex_lock_timeout_sec); //in case Owner of this lock is dead/no Lock condition then the alarm is generated for this instance and kill this process.
	CLOG("[SHM] Before locking in pid=%d\n", getpid());
	rv = pthread_mutex_lock(&(pstLock->mtx_lock));
	// while(mutex_lock_timeout){
	// 	alarm(config.mutex_lock_timeout_sec); //in case Owner of this lock is dead the alarm is generated for this instance and it will unlock the shared lock
	// 	CLOG("[ALM] Err Before locking in pid=%d\n", getpid());
	// 	mutex_lock_timeout = 0;
	// 	rv = pthread_mutex_lock(&(stMy_data->mtxlock));
	// }
	
	
	CLOG("Lock Acquired[pid=%d], count: %d, rv: %d\n", getpid(), ++stMy_data->st_shm_data.count, rv);
	if(rv == EOWNERDEAD)
	{
		DLOG("Owner Dead identified by : %ld\n",pthread_self());
		/* Reset shared data and mutex to valid state */
		recover(pstLock);
	}
	pstLock->pid = getpid();
}


void shared_mem_mutex_unlock(struct stMutLock *pstLock){
	
	CLOG("Counter[%d]: %d\n", getpid(), stMy_data->st_shm_data.count);
	
	pthread_mutex_unlock(&(pstLock->mtx_lock));
	pstLock->pid = -1;
	CLOG("After unlocking in pid=%d\n", getpid());

}


void sighand(int no)
{
	// CLOG("I am in sighandler %d, pid: %d \n", no, getpid());
	// if(no == SIGALRM){
	// 	CLOG("SIGALARM my pid: %d, locked by: %d\n",getpid(),stMy_data->pid);
	// 	if(getpid() != stMy_data->pid && stMy_data->pid != -1){ //Here you can check if the stMy_data->pid is still running
	// 		shared_mem_mutex_unlock();
	// 	}
	// 	mutex_lock_timeout = 1;
	// } else if((no == 2 || no == 4) && getpid() == stMy_data->pid){ //kill signal and if I have locked the resource then unlock it
	// 	CLOG("[%d] Signal my pid: %d, locked by: %d\n", no, getpid(),stMy_data->pid);
	// 	pthread_mutex_unlock(&(stMy_data->mtxlock));
	// } else {
	// 	_exit(EXIT_FAILURE);	/* Can't return after SIGSEGV */
	// }
	_exit(EXIT_FAILURE);
}



int create_mutex_datastruct(char *file_name, int isExists)
{
	int fd;
	unsigned int val;
	pthread_mutexattr_t pshared;
	DLOG("SHM file %s, exists: %d\n", file_name, isExists);
	int len = getpagesize();
	if(isExists != 0){ //When file does not exists
		pthread_mutexattr_init(&pshared);

		pthread_mutexattr_setpshared(&pshared, PTHREAD_PROCESS_SHARED);
		// pthread_mutexattr_settype(&pshared, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutexattr_setrobust(&pshared, PTHREAD_MUTEX_ROBUST);
		
		pthread_mutexattr_getpshared(&pshared , &val);
		DLOG("pShared Val %d\n", val);

		// pthread_mutexattr_gettype(&pshared , &val);
		// DLOG("pTypes Val %d\n", val);

		pthread_mutexattr_getrobust(&pshared , &val);
		DLOG("pRobust Val %d\n", val);
		
		DLOG("File_created: %s\r\n", file_name);

		fd = shm_open(file_name, O_CREAT | O_RDWR, 0666);
		if (fd == -1) {
			perror("shm_open");
			exit(1);
		}

		/* 4k is min shared memory */
		if (ftruncate(fd, len) == -1) {
			perror("ftruncate");
			exit(1);
		}
	} else {
		DLOG("Opening a file: %s\r\n", file_name);
		fd = shm_open(file_name,  O_RDWR, 0);
		if (fd == -1) {
			perror("shm_open");
			exit(1);
		}
		/* Use shared memory object size as length argument for mmap()
       	and as number of bytes to write() */
		if (ftruncate(fd, len) == -1) {
			perror("ftruncate");
			exit(1);
		}

	}
	
	stMy_data = (struct mydata *)mmap(NULL, len,
				  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (stMy_data == MAP_FAILED) 
        perror("mmap"); 
	if (close(fd) == -1)                    /* 'fd' is mo longer needed */ 
        perror("close"); 
		
	//if it was not present then initialize everything again
	if(isExists != 0){
		CLOG("Initializing SHM data......\n");

		//Init the structure members here
		memset((char *)&stMy_data->st_shm_data, 0, sizeof(stMy_data->st_shm_data));

		//Mutex init...
		pthread_mutex_init(&stMy_data->stAnalog_lock.mtx_lock, &pshared);
		pthread_mutex_init(&stMy_data->stModbus_lock.mtx_lock, &pshared);
	}
	return 0;
}



int shared_mem_init(char *file_name){

	struct stat st_fstate;
	char shared_file[256];

	signal(3, sighand);
	signal(11, sighand);
	signal(4, sighand);
	signal(2, sighand);
	signal(SIGALRM, sighand);

	sprintf(shared_file, "%s%s", SHARED_MEM_PATH, file_name);
    int exists = stat(shared_file, &st_fstate);
	// CLOG("File %s existss: %d\n", shared_file, exists);
	create_mutex_datastruct(file_name, exists); //file does not exists, create a shared memory and mutex
	return 1;
}


/**
 * @brief to unmapped the memory, this is necessary if process is continuously running else on exit these section will be unmapped
 * 
 * @param file_name 
 * @return int 
 */
void shared_mem_deinit(void)
{
	munmap(stMy_data, sizeof(struct mydata));
}

void system_init(){
	config_startup();
    shared_mem_init(SHARED_MEM_FILE_NAME); 
}

void system_Deinit(){
  shared_mem_deinit(); 

}

