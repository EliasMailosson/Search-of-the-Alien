#ifndef THREAD_H
#define THREAD_H

#ifdef _WIN32
#include <windows.h>
typedef HANDLE thread_t;
typedef CRITICAL_SECTION mutex_t;
#else
#include <pthread.h>
typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
#endif

typedef void* (*thread_func_t)(void*);

// Thread functions
int thread_create(thread_t* thread, thread_func_t func, void* arg);
int thread_join(thread_t thread);

// Mutex functions
int mutex_init(mutex_t* mutex);
int mutex_lock(mutex_t* mutex);
int mutex_unlock(mutex_t* mutex);
int mutex_destroy(mutex_t* mutex);

#endif // THREAD_H
