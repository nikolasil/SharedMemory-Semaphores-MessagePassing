#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "util.h"	   // a library that has some general function and defines
#include "semaphore.h" // a library that has all the semaphore things

struct shared_memory_segment_st
{
	int shm_id;	   // the id of the shm
	int semaphore; // the id of the semaphores
	int p2_pid;	   // the id of the p2 proccess
	int chan_pid;  // the id of the channel proccess
	int tries;	   // the number of tries to sent the message

	int isProducer;			   // if this variable has value 1->(the process 1 is the producer), if it has 2->(the process 2 is the producer), it has -1->(none is the producer)
	char message[BUFFER];	   // it holds the message that is beeing typed by the user so i dont loose it
	char line[2 * BUFFER]; // this is the "line" that connects the ENC1 AND ENC2
};

struct shared_memory_segment_st *createSharedMemory(); // creates the shm

void shm_destroy(struct shared_memory_segment_st *shared_memory_segment); // destroys the shm

void setProducer(struct shared_memory_segment_st *shared_memory_segment, int i); // set the producer = i

void setMessage(struct shared_memory_segment_st *shared_memory_segment, char mes[BUFFER]); // set the message = mes

void setSemaphore(struct shared_memory_segment_st *shared_memory_segment, int semid); // set semaphore = semid

void setSignature(struct shared_memory_segment_st *shared_memory_segment, char *signature); // go to the location of the signature in the line and change it

void clearLine(struct shared_memory_segment_st *shared_memory_segment); // clear the line

void create_Line_Check_Sign(struct shared_memory_segment_st *shared_memory_segment, char *message, char *check, char *signature); // line = message + '\0' + '\0' + checksum + '\0' + '\0' + signature + '\0' + '\0' + '\0' . . .

int getProducer(struct shared_memory_segment_st *shared_memory_segment); // get the variable producer

char *getMessage(struct shared_memory_segment_st *shared_memory_segment); // get the message

char *getLine(struct shared_memory_segment_st *shared_memory_segment); // get the line

int getSemaphore(struct shared_memory_segment_st *shared_memory_segment); // get the semaphore

char *getMessageFromTheLine(struct shared_memory_segment_st *shared_memory_segment); // get the message from the line

char *getChecksumFromTheLine(struct shared_memory_segment_st *shared_memory_segment); // get the checksum from the line.

char *getSignatureFromTheLine(struct shared_memory_segment_st *shared_memory_segment); // get the signature from the line.
#endif