#ifndef UTILITY_H
#define UTILITY_H

#define P1 1
#define P2 2

#define BUFFER 512

#define KEYSHM 1115201800332

#define numOfSemaphores 5
#define SEM_P1 0
#define SEM_ENC1 1
#define SEM_CHAN 2
#define SEM_ENC2 3
#define SEM_P2 4

#include "semaphore.h"     // a library that has all the semaphore things
#include "shared_memory.h" // a library that has all the shared memory segment things

char input_p1(struct shared_memory_segment_st *shared_memory_segment);
char input_p2(struct shared_memory_segment_st *shared_memory_segment);
void typed(struct shared_memory_segment_st *shared_memory_segment);
int probability(double prob);

#endif