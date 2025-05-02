#include "../../include/CONCURRENCY/semaphore.h"

#ifdef _WIN32

int semaphore_init(semaphore_t* sem, unsigned int value) {
    *sem = CreateSemaphore(NULL, value, LONG_MAX, NULL);
    return *sem ? 0 : -1;
}

int semaphore_wait(semaphore_t* sem) {
    return WaitForSingleObject(*sem, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
}

int semaphore_post(semaphore_t* sem) {
    return ReleaseSemaphore(*sem, 1, NULL) ? 0 : -1;
}

int semaphore_destroy(semaphore_t* sem) {
    return CloseHandle(*sem) ? 0 : -1;
}

#else  // POSIX fallback with pthreads

int semaphore_init(semaphore_t* sem, unsigned int value) {
    if (pthread_mutex_init(&sem->mutex, NULL) != 0) return -1;
    if (pthread_cond_init(&sem->cond, NULL) != 0) {
        pthread_mutex_destroy(&sem->mutex);
        return -1;
    }
    sem->value = value;
    return 0;
}

int semaphore_wait(semaphore_t* sem) {
    pthread_mutex_lock(&sem->mutex);
    while (sem->value == 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int semaphore_post(semaphore_t* sem) {
    pthread_mutex_lock(&sem->mutex);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int semaphore_destroy(semaphore_t* sem) {
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    return 0;
}

#endif
