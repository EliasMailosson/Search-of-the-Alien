#include "../../include/CONCURRENCY/threads.h"
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>

typedef struct {
    thread_func_t func;
    void* arg;
} thread_args_t;

static DWORD WINAPI windows_thread_func(LPVOID arg) {
    thread_args_t* args = (thread_args_t*)arg;
    thread_func_t real_func = args->func;
    void* real_arg = args->arg;
    free(args);
    void* result = real_func(real_arg);
    return (DWORD)(uintptr_t)result;
}

int thread_create(thread_t* thread, thread_func_t func, void* arg) {
    thread_args_t* args = malloc(sizeof(thread_args_t));
    if (!args) return -1;
    args->func = func;
    args->arg  = arg;
    *thread = CreateThread(NULL, 0, windows_thread_func, args, 0, NULL);
    return *thread != NULL ? 0 : -1;
}

int thread_join(thread_t thread) {
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    return 0;
}

int mutex_init(mutex_t* mutex) {
    InitializeCriticalSection(mutex);
    return 0;
}

int mutex_lock(mutex_t* mutex) {
    EnterCriticalSection(mutex);
    return 0;
}

int mutex_unlock(mutex_t* mutex) {
    LeaveCriticalSection(mutex);
    return 0;
}

int mutex_destroy(mutex_t* mutex) {
    DeleteCriticalSection(mutex);
    return 0;
}

#else  // POSIX

int thread_create(thread_t* thread, thread_func_t func, void* arg) {
    return pthread_create(thread, NULL, func, arg);
}

int thread_join(thread_t thread) {
    return pthread_join(thread, NULL);
}

int mutex_init(mutex_t* mutex) {
    return pthread_mutex_init(mutex, NULL);
}

int mutex_lock(mutex_t* mutex) {
    return pthread_mutex_lock(mutex);
}

int mutex_unlock(mutex_t* mutex) {
    return pthread_mutex_unlock(mutex);
}

int mutex_destroy(mutex_t* mutex) {
    return pthread_mutex_destroy(mutex);
}

#endif
