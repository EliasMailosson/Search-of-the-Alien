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
    #if defined(__clang__)
    # pragma clang diagnostic push
    # pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif

    int result = sem_init(sem, 0, value);

    #if defined(__clang__)
    # pragma clang diagnostic pop
    #endif

    return result;
}

int semaphore_wait(semaphore_t* sem) {
    return sem_wait(sem);
}

int semaphore_post(semaphore_t* sem) {
    return sem_post(sem);
}

int semaphore_destroy(semaphore_t* sem) {
    #if defined(__clang__)
    # pragma clang diagnostic push
    # pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif

    int result = sem_destroy(sem);

    #if defined(__clang__)
    # pragma clang diagnostic pop
    #endif

    return result;
}

#endif