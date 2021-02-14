#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "util.h"          // a library that has some general function and defines
#include "shared_memory.h" // a library that has all the shared memory segment things

int sem_create(int num_semaphores);             // create n = num_semaphores numberof semaphores
void sem_destroy(int semid);                    // destroy the semaphores
void sem_init(int semid, int index, int value); // set the value of a semaphore with the spesific index
void sem_wait(int semid, int index);            // wait a semaphore with the spesific index
void sem_signal(int semid, int index);          // signal a semaphore with the spesific index

#endif