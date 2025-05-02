#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <stdint.h>

#ifdef _WIN32

#include <windows.h>
typedef HANDLE semaphore_t;

#else  // POSIX fallback

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    unsigned int    value;
} semaphore_t;

#endif

// returns 0 on success, -1 on error
int semaphore_init(semaphore_t* sem, unsigned int value);
int semaphore_wait(semaphore_t* sem);
int semaphore_post(semaphore_t* sem);
int semaphore_destroy(semaphore_t* sem);


#endif
