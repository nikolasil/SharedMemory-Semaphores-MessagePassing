#include <stdio.h> //  for input and output on the terminal
#include <stdlib.h>
#include <pthread.h> // for creating threads
#include <string.h>  //  for string manipulation
#include <sys/types.h>

#include "shared_memory.h" // a library that has all the shared memory segment things
#include "semaphore.h"     // a library that has all the semaphore things
#include "util.h"          // a library that has some general function and defines

int main(int argc, char *argv[])
{
    time_t t;
    srand((unsigned)time(&t));

    struct shared_memory_segment_st *shared_memory_segment = createSharedMemory();
    shared_memory_segment->chan_pid = getpid();
    printf("[CHAN] Running\n");
    if (argc > 2)
    {
        printf("[CHAN] Too many arguments supplied\n");
        return 0;
    }
    else if (argc == 1)
    {
        printf("[CHAN] You have to give a probability as an argument\n");
        return 0;
    }
    while (1) // do a loop while the message transfered != "TEMP"
    {
        printf("[CHAN] I wait for a signal\n");
        sem_wait(getSemaphore(shared_memory_segment), SEM_CHAN); // wait
        printf("[CHAN] Signaled\n");

        printf("[CHAN] I take the signature from the line\n");
        char *signature = getSignatureFromTheLine(shared_memory_segment);

        if (getProducer(shared_memory_segment) == P1) // this means that if the p1 is the producer the flow is from ecn1 -> enc2
        {
            if (strcmp(signature, "SENT") == 0) // that means that the flow is from enc1->enc2
            {
                printf("[CHAN] Signature = SENT\n");
                printf("[CHAN] I will start transmitting the message to the ENC2\n");
                char *message = getLine(shared_memory_segment);
                int i = 0;
                printf("|");
                while (i <= strlen(message))
                {
                    if (probability(strtod(argv[1], NULL)) == 1) // if the probability = 1 then change the character
                    {
                        message[i] = '_';
                    }
                    printf("%c", message[i]);
                    i++;
                }
                printf("| 100%%\n");
                printf("[CHAN] Message has been trasmitted\n");
                printf("[CHAN] I will singal ENC2\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC2);
            }
            else if (strcmp(signature, "REDO") == 0) // that means that i have sent to enc2 but the message didnt managed to get there undamaged so i have to notify enc1 to sent it again
            {
                printf("[CHAN] Signature = REDO\n");
                printf("[CHAN] The message was received damaged by the ENC2 so i will signal ENC1 to sent it again\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC1);
            }
            else if (strcmp(signature, "DONE") == 0) // that means that i have sent to enc2 and the message managed to get there undamaged so i have to notify enc1 that it has been sent
            {
                printf("[CHAN] Signature = DONE\n");
                printf("[CHAN] The message was received undamaged by the ENC2 so i will signal ENC1 that is has been sent\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC1);
                if (strcmp(getLine(shared_memory_segment), "TEMP") == 0)
                {
                    printf("[CHAN] The message int the line was TEMP. I will terminate\n");
                    return 0;
                }
            }
        }
        else // this means that if the p1 is the producer the flow is from ecn2 -> enc1
        {
            if (strcmp(signature, "SENT") == 0) // that means that the flow is from enc2->enc1
            {
                printf("[CHAN] Signature = SENT\n");
                printf("[CHAN] I will start transmitting the message to the ENC1\n");
                char *message = getLine(shared_memory_segment);
                int i = 0;
                printf("|");
                while (i <= strlen(message))
                {
                    if (probability(strtod(argv[1], NULL)) == 1) // if the probability = 1 then change the character
                    {
                        message[i] = '_';
                    }
                    printf("%c", message[i]);
                    i++;
                }
                printf("| 100%%\n");
                printf("[CHAN] Message has been trasmitted\n");
                printf("[CHAN] I will singal ENC1\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC1);
            }
            else if (strcmp(signature, "REDO") == 0) // that means that i have sent to enc1 but the message didnt managed to get there undamaged so i have to notify enc2 to sent it again
            {
                printf("[CHAN] Signature = REDO\n");
                printf("[CHAN] The message was received damaged by the ENC1 so i will signal ENC2 to sent it again\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC2);
            }
            else if (strcmp(signature, "DONE") == 0) // that means that i have sent to enc1 and the message managed to get there undamaged so i have to notify enc2 that it has been sent
            {
                printf("[CHAN] Signature = DONE\n");
                printf("[CHAN] The message was received undamaged by the ENC1 so i will signal ENC2 that is has been sent\n");
                sem_signal(getSemaphore(shared_memory_segment), SEM_ENC2);
                if (strcmp(getLine(shared_memory_segment), "TEMP") == 0)
                {
                    printf("[CHAN] The message was TEMP. I will terminate\n");
                    return 0;
                }
            }
        }
    }
    return 0;
}