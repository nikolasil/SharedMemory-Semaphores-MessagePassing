#include <sys/sem.h>    //  for creating semaphores
#include <stdlib.h>     //  for general purpose
#include <stdio.h>      //  for input and output on the terminal

#include "util.h"	// a library that has some general function and defines
#include "semaphore.h"	// a library that has all the semaphore things
#include "shared_memory.h"	// a library that has all the shared memory segment things

int sem_create(int num_semaphores)
{  /* procedure to create specified number of semaphores */
   int semid;

   /* create new semaphore set of semaphores */
   if ((semid = semget (IPC_PRIVATE, num_semaphores, IPC_CREAT | 0600)) < 0)
     {  printf("error in creating semaphore");/* 0600 = read/alter by user */
        exit (1);
     }
   return semid;
}

void sem_destroy(int semid){
   if (semctl (semid, 0, IPC_RMID) < 0)
     {  perror ("error in removing semaphore from the system");
        exit (1);
     }
}

void sem_init(int semid, int index, int value)
{  /* procedure to initialize specified semaphore to given value */
   if (semctl (semid, index, SETVAL, value) < 0)
     {  printf("error in initializing first semaphore");
        exit (1);
     }
}

void sem_wait(int semid, int index)
{/* procedure to perform a P or wait operation on a semaphore of given index */
  struct sembuf sops[1];  /* only one semaphore operation to be executed */

   sops[0].sem_num = index;/* define operation on semaphore with given index */
   sops[0].sem_op  = -1;   /* subtract 1 to value for P operation */
   sops[0].sem_flg = 0;    /* type "man semop" in shell window for details */

   if (semop (semid, sops, 1) == -1)
     {  printf("sem_wait error in semaphore operation");
        exit (1);
     }
}

void sem_signal(int semid, int index)
{/* procedure to perform a V or signal operation on semaphore of given index */
   struct sembuf sops[1];  /* define operation on semaphore with given index */

   sops[0].sem_num = index;/* define operation on semaphore with given index */
   sops[0].sem_op  = 1;    /* add 1 to value for V operation */
   sops[0].sem_flg = 0;    /* type "man semop" in shell window for details */

   if (semop (semid, sops, 1) == -1)
     {  printf("sem_signal error in semaphore operation");
        exit (1);
     }
}