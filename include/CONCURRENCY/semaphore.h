#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _WIN32
#include <windows.h>
typedef HANDLE semaphore_t;
#else
#include <semaphore.h>
typedef sem_t semaphore_t;
#endif

// Semaphore functions
int semaphore_init(semaphore_t* sem, unsigned int value);
int semaphore_wait(semaphore_t* sem);    // waits until >0, then decrements
int semaphore_post(semaphore_t* sem);    // increments
int semaphore_destroy(semaphore_t* sem);

#endif // SEMAPHORE_H

