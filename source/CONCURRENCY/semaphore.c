#include "../../include/CONCURRENCY/semaphore.h"

#ifdef _WIN32

int semaphore_init(semaphore_t* sem, unsigned int value) {
    *sem = CreateSemaphore(NULL, value, LONG_MAX, NULL);
    return *sem != NULL ? 0 : -1;
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

#else // POSIX

int semaphore_init(semaphore_t* sem, unsigned int value) {
    return sem_init(sem, 0, value); // 0 = shared between threads
}

int semaphore_wait(semaphore_t* sem) {
    return sem_wait(sem);
}

int semaphore_post(semaphore_t* sem) {
    return sem_post(sem);
}

int semaphore_destroy(semaphore_t* sem) {
    return sem_destroy(sem);
}

#endif

