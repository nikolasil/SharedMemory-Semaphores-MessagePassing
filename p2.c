#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <openssl/md5.h>

#include "shared_memory.h" // a library that has all the shared memory segment things
#include "semaphore.h"	   // a library that has all the semaphore things
#include "util.h"		   // a library that has some general function and defines

int main(void)
{
	printf("[P2] Running\n");
	struct shared_memory_segment_st *shared_memory_segment = createSharedMemory(); // create the shm
	char *message;																   // make a pointer to hold the message
	do
	{															  // a loop to make the programm repeat it self until the message is "TEMP"
		setProducer(shared_memory_segment, -1);					  // set the producer to none
		sem_init(getSemaphore(shared_memory_segment), SEM_P1, 1); // wait
		sem_init(getSemaphore(shared_memory_segment), SEM_ENC1, 0);
		sem_init(getSemaphore(shared_memory_segment), SEM_CHAN, 0);
		sem_init(getSemaphore(shared_memory_segment), SEM_ENC2, 0);
		sem_init(getSemaphore(shared_memory_segment), SEM_P2, 1); // wait

		printf("[P2] Type a message to me or in the P1[Write TEMP to terminate]: ");

		pthread_t thread_id_input;
		pthread_create(&thread_id_input, NULL, input_p2, shared_memory_segment); // make a thread to do scanf
		// so i can kill the thread when i dont want to scanf anymore because the user have given the message to the p1

		pthread_t thread_id_typed;
		pthread_create(&thread_id_typed, NULL, typed, shared_memory_segment); // make a thread to check if anything has been typed to me or in the p1

		pthread_join(thread_id_typed, NULL); // wait for the typed to end so i know that something has been typed to me or p1

		pid_t pid = fork(); // make the child[ENC2]
		if (pid == -1)
		{ // check
			printf("[P2] Error in fork() for ENC2");
		}
		if (pid == 0)
		{												  //----------------- [ENC2] Child ----------------------
			if (getProducer(shared_memory_segment) == P2) // the code for the child if the producer is P2
			{
				printf("[ENC2] Running\n");
				clearLine(shared_memory_segment); // clear the line
				shared_memory_segment->tries = 0;
				while (1)						  // do a loop until the message has arrived
				{
					printf("[ENC2] Waiting for P2 or CHAN to signal\n");
					sem_wait(getSemaphore(shared_memory_segment), SEM_ENC2); // wait semaphore SEM_ENC2 then work
					shared_memory_segment->tries++;
					printf("[ENC2] P2 or CHAN signaled\n");

					char *sign = getSignatureFromTheLine(shared_memory_segment); // check the signature to see if the message has arrived
					if (strcmp(sign, "DONE") == 0)								 // if true break while
					{
						printf("[ENC2] The message was successfully sent after %d tries\n",shared_memory_segment->tries-1);
						break;
					}

					printf("[ENC2] I create the checksum for the message\n"); // create the checksum
					unsigned char *checksum = MD5(getMessage(shared_memory_segment), strlen(getMessage(shared_memory_segment)), NULL);

					printf("[ENC2] Checksum of message: %s\n", checksum);

					printf("[ENC2] I store the message+checksum in the line with a SENT signature\n"); // line = message + '\0' + '\0'  + checksum + '\0' + '\0'  + signature + '\0' + '\0' + '\0' . . .
					create_Line_Check_Sign(shared_memory_segment, getMessage(shared_memory_segment), checksum, "SENT");

					printf("[ENC2] I singal CHAN\n");
					sem_signal(getSemaphore(shared_memory_segment), SEM_CHAN); // signal semaphore SEM_CHAN
				}
				printf("[ENC2] Finished\n");
				exit(0);
			}
			else // the code for the child if the producer is P1
			{
				printf("[ENC2] Running\n");
				while (1) // do a loop until the message is correct
				{
					printf("[ENC2] Waiting for CHAN to transmit the message and signal me\n");
					sem_wait(getSemaphore(shared_memory_segment), SEM_ENC2); // wait semaphore SEM_ENC2 so hat P2 has done his thing
					printf("[ENC2] Signaled. The CHAN transmited the message\n");

					printf("[ENC2] Message received: %s\n", getLine(shared_memory_segment));
					printf("[ENC2] I create the checksum for the message in the line\n"); // create the checksum of the message received
					unsigned char *checksum1 = MD5(getLine(shared_memory_segment), strlen(getLine(shared_memory_segment)), NULL);
					printf("[ENC2] Checksum of message: %s\n", checksum1);

					printf("[ENC2] I take the checksum from the line to do a comparison\n");
					unsigned char *checksum2 = getChecksumFromTheLine(shared_memory_segment);

					if (strcmp(checksum1, checksum2) == 0) // the message has been transfered correctly
					{
						printf("[ENC2] The checksums were the same so the message is correct\n");
						printf("[ENC2] I have to notify CHAN that the message was correct\n");

						printf("[ENC2] I change the signature of the line with DONE\n");
						setSignature(shared_memory_segment, "DONE");
						printf("[ENC2] I signal CHAN\n");
						sem_signal(getSemaphore(shared_memory_segment), SEM_CHAN); // signal CHAN with the signature = DONE to notify that the message was correct so the chan notifies enc2 to break the loop

						printf("[ENC2] I signal P1 to take the message\n");
						sem_signal(getSemaphore(shared_memory_segment), SEM_P2); // tell p2 to take the message
						break;
					}
					else // wrong message
					{
						printf("[ENC2] The checksums were NOT the same so the message is incorrect\n");
						printf("[ENC2] I signal CHAN to resend the message with a signature REDO\n");

						printf("[ENC2] I change the signature of the line with REDO\n");
						setSignature(shared_memory_segment, "REDO"); // signal CHAN with the signature = REDO to notify that the message was INcorrect so the chan notifies enc2 to sent it again
						printf("[ENC2] I signal CHAN to resend the message\n");
						sem_signal(getSemaphore(shared_memory_segment), SEM_CHAN);
					}
				}
				printf("[ENC2] Finished\n");
				exit(0);
			}
			exit(0);
		}
		else
		{ //--------------------- [P2] Father -----------------------
			if (getProducer(shared_memory_segment) == P2)
			{ // we have already make the semaphore SEM_P2 wait in the input
				printf("[P2] I am the producer and the P2 became the consumer\n");
				pthread_join(thread_id_input, (void **)&message); // take the input that the user gave and store it to message

				printf("[P2] I saved the messsage in the shared memeory segment\n");

				setMessage(shared_memory_segment, message); // store the message to the shm

				printf("[P2] I signal ENC1\n");
				sem_signal(getSemaphore(shared_memory_segment), SEM_ENC2); // signal semaphore SEM_ENC2 because we are finished

				printf("[P2] I wait for ENC2 to finish\n");
				waitpid(pid, NULL, 0); // When child finished it means that the message was arrived at p1
				printf("[P2] ENC1 Finished\n");
			}
			else
			{
				printf("\n[P2] P2 is the producer and i became the consumer\n");
				pthread_cancel(thread_id_input);
				printf("[P2] I wait for the ENC2 signal me\n");
				sem_wait(getSemaphore(shared_memory_segment), SEM_P2);
				printf("[P2] ENC2 singaled me\n");

				printf("[P2] I get the message from the line\n");
				message = getMessageFromTheLine(shared_memory_segment);
				printf("[P2] The message: %s\n", message);
			}
		}

		if (strcmp(message, "TEMP") == 0)
		{
			printf("[P2] The message was TEMP. I will terminate\n");
			if (getProducer(shared_memory_segment) == P2)
			{
				free(message);
			}
			return 0;
		}
		if (getProducer(shared_memory_segment) == P2)
		{
			free(message);
		}
	} while (1);
}