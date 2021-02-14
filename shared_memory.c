#include <sys/shm.h> //  for shared memory
#include <stdlib.h>	 //  for general purpose
#include <stdio.h>	 //  for input and output on the terminal
#include <string.h>	 //  for string manipulation

#include "util.h"		   // a library that has some general function and defines
#include "semaphore.h"	   // a library that has all the semaphore things
#include "shared_memory.h" // a library that has all the shared memory segment things

struct shared_memory_segment_st *createSharedMemory()
{
	int shmid = shmget((key_t)KEYSHM, sizeof(struct shared_memory_segment_st), 0666 | IPC_CREAT); // communication
	if (shmid == -1)
	{ // check
		printf("shmget failed\n");
		exit(EXIT_FAILURE);
	}
	void *shared_memory = shmat(shmid, (void *)0, 0); // get pointer for shm
	if (shared_memory == (void *)-1)
	{ // check
		printf("shmat failed\n");
		exit(EXIT_FAILURE);
	}
	struct shared_memory_segment_st *shared_memory_segment = (struct shared_memory_segment_st *)shared_memory;
	shared_memory_segment->isProducer = -1;
	int i = 0;
	while (i <= BUFFER)
	{
		shared_memory_segment->message[i] = '\0';
		i++;
	}
	i = 0;
	while (i <= 2 * BUFFER)
	{
		shared_memory_segment->line[i] = '\0';
		i++;
	}
	shared_memory_segment->shm_id = shmid;
	return shared_memory_segment;
}

void shm_destroy(struct shared_memory_segment_st *shared_memory_segment)
{
	int id = shared_memory_segment->shm_id;
	shmdt(shared_memory_segment);
	shmctl(id, IPC_RMID, NULL);
}

void setProducer(struct shared_memory_segment_st *shared_memory_segment, int i)
{
	shared_memory_segment->isProducer = i;
}

void setMessage(struct shared_memory_segment_st *shared_memory_segment, char *mes)
{
	strcpy(shared_memory_segment->message, mes);
}

void setSemaphore(struct shared_memory_segment_st *shared_memory_segment, int semid)
{
	shared_memory_segment->semaphore = semid;
}

void setSignature(struct shared_memory_segment_st *shared_memory_segment, char *signature)
{
	char *sign = getSignatureFromTheLine(shared_memory_segment);
	sign[0] = signature[0];
	sign[1] = signature[1];
	sign[2] = signature[2];
	sign[3] = signature[3];
	sign[4] = '\0';
	sign[5] = '\0';
}

void clearLine(struct shared_memory_segment_st *shared_memory_segment)
{
	int i = 0;
	while (i <= 2 * BUFFER)
	{
		shared_memory_segment->line[i] = '\0';
		i++;
	}
}

void create_Line_Check_Sign(struct shared_memory_segment_st *shared_memory_segment, char *message, char *check, char *signature)
{
	strcpy(shared_memory_segment->line, message);
	int i = 0;

	while (i <= BUFFER)
	{
		if (shared_memory_segment->line[i] == '\0')
		{
			break;
		}
		i++;
	}

	i++;
	shared_memory_segment->line[i] = '\0';
	i++;
	int j = 0;
	while (j <= strlen(check))
	{
		shared_memory_segment->line[i] = check[j];
		i++;
		j++;
	}
	i--;
	shared_memory_segment->line[i] = '\0';
	i++;
	shared_memory_segment->line[i] = '\0';
	i++;
	j = 0;
	while (j <= strlen(signature))
	{
		shared_memory_segment->line[i] = signature[j];
		i++;
		j++;
	}
	i--;
	shared_memory_segment->line[i] = '\0';
	shared_memory_segment->line[i+1] = '\0';
}

int getProducer(struct shared_memory_segment_st *shared_memory_segment)
{
	return shared_memory_segment->isProducer;
}

char *getMessage(struct shared_memory_segment_st *shared_memory_segment)
{
	return shared_memory_segment->message;
}

char *getLine(struct shared_memory_segment_st *shared_memory_segment)
{
	return shared_memory_segment->line;
}

int getSemaphore(struct shared_memory_segment_st *shared_memory_segment)
{
	return shared_memory_segment->semaphore;
}

char *getMessageFromTheLine(struct shared_memory_segment_st *shared_memory_segment)
{
	return shared_memory_segment->line;
}

char *getChecksumFromTheLine(struct shared_memory_segment_st *shared_memory_segment)
{
	int i = 0;

	while (i <= BUFFER)
	{
		if (shared_memory_segment->line[i] == '\0')
		{
			break;
		}
		i++;
	}
	char *ptr = &shared_memory_segment->line[i + 2];
	return ptr;
}

char *getSignatureFromTheLine(struct shared_memory_segment_st *shared_memory_segment)
{
	int i = 0;
	int count = 0;
	while (i <= BUFFER)
	{
		if (shared_memory_segment->line[i] == '\0')
		{
			count++;
			if (count == 3)
			{
				break;
			}
		}
		i++;
	}
	char *ptr = &shared_memory_segment->line[i + 2];
	return ptr;
}