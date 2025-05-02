#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef HANDLE thread_t;
typedef CRITICAL_SECTION mutex_t;

static inline void sleep_ms(int ms) { Sleep(ms); }

static inline uintptr_t thread_self(void) {
        return (uintptr_t)GetCurrentThreadId();
}

#else
#include <pthread.h>
#include <unistd.h>
typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;

static inline void sleep_ms(int ms) { usleep(ms * 1000); }

static inline uintptr_t thread_self(void) {
    return (uintptr_t)pthread_self();
}

#endif

typedef void* (*thread_func_t)(void*);

int thread_create(thread_t* thread, thread_func_t func, void* arg);
int thread_join(thread_t thread);
int mutex_init(mutex_t* mutex);
int mutex_lock(mutex_t* mutex);
int mutex_unlock(mutex_t* mutex);
int mutex_destroy(mutex_t* mutex);

int mutex_trylock(mutex_t* mutex);

#endif 
