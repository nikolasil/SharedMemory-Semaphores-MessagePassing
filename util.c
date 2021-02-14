#include <stdio.h> //  for input and output on the terminal
#include <stdlib.h>
#include <pthread.h> // for creating threads
#include <string.h>	 //  for string manipulation

#include "util.h"		   // a library that has some general function and defines
#include "semaphore.h"	   // a library that has all the semaphore things
#include "shared_memory.h" // a library that has all the shared memory segment things

char input_p1(struct shared_memory_segment_st *shared_memory_segment)
{
	char input_string[BUFFER];
	scanf(" %[^\n]s", &input_string);
	char *space = (char *)malloc(BUFFER);
	strcpy(space, input_string);
	sem_wait(getSemaphore(shared_memory_segment), SEM_P1);
	sem_wait(getSemaphore(shared_memory_segment), SEM_P2);
	setProducer(shared_memory_segment, P1);
	pthread_exit(space);
}

char input_p2(struct shared_memory_segment_st *shared_memory_segment)
{
	char input_string[BUFFER];
	scanf(" %[^\n]s", &input_string);
	char *space = (char *)malloc(BUFFER);
	strcpy(space, input_string);
	sem_wait(getSemaphore(shared_memory_segment), SEM_P1);
	sem_wait(getSemaphore(shared_memory_segment), SEM_P2);
	setProducer(shared_memory_segment, P2);
	pthread_exit(space);
}

void typed(struct shared_memory_segment_st *shared_memory_segment)
{
	while (1)
	{
		if (getProducer(shared_memory_segment) == P1 || getProducer(shared_memory_segment) == P2)
		{
			break;
		}
	}
}

int probability(double prob)
{
	double r = rand() % 101;
	r = r / 100;
	if (r < prob)
	{
		return 1;
	}
	return 0;
}