My files are: p1.c, p2.c, channel.c, shared_memory.c, shared_memory.h, semaphore.c, semaphore.h, util.c, util.h
For compile and run i have make a Makefile. You can use the command(make all) to create the p1, p2, chan.

Running rules:

    1)  The first process that has to be run is p1, because p1 creates the semaphores and then we can run chan and p2.

    2)  We have to use 3 terminals.     terminal 0 for the p1
                                        terminal 1 for the chan
                                        terminal 2 for the p2

    3)  The argument for the probability in the channel must be in the format 0.1 , 0.2, 0.3 . . .
        where 0.1 is 10% 0.2 is 20% . . .

General Information:

    1)  All the processes run in a while until TEMP is input. The TEMP will first transfer and then the processes will terminate.

    2)  I noticed that scanf() can't stop checking for input after something happend.
        So in order to have p1 and p2 use scanf() at the same time and when for example in p2 the user writes something then the p1 will not longer do scanf().
        I created threads to do the scanf() so i can kill the thread when i dont need to do scanf() anymore.

        And i used and an other thread to just check if on the other 2 threads an input had taken in order to terminate the other thread and take the input.

    3)  Because i wanted to make the programm more practical i made an integer in the shared memory called isProducer so i know the flow that the message has to go.
        And each process will do different things when the producer is P1 or P2.
    
    4) I have in the shared memory the shm_id and the sem_id for practicallity when i want to use the semaphores or destroy semaphores and shared memory

    5) I have in the shared memory the process id of the channel and p2

Message Transfer:

    I wanted to transfer only one thing throw the channel. And not multiply strings.
    So i made a string named line in the shared memory that it will be in the form: message + '\0' + '\0' + checksum + '\0' + '\0' + signature + '\0' + '\0' . . .

    message:    is the message that we want to transfer and it is the only thing that the channel will change based on the probability.
    checksum:   is the checksum of the message so the receiver ENC can compare it with the checksum that it will generate from the message that it received
    signature:  is either "SENT", "REDO", "DONE". The purpose of this is to make a communication between the two ENC.

    for the channel:
        "SENT"->follow the flow
        "REDO"->ask for the ENC that sents the message to make sent the line again
        "DONE"->just notify the ENC that sents the message to exit his while

    for the ENC that sents the message:
        i put "SENT" when i sent the message
        "REDO"->resend the message with "SENT" signature
        "DONE"->exit while

    for the ENC that receives the message:
        i put "DONE" when i the message is correct
        i put "REDO" when i the message is incorrect

p1.c:
    This file contain the code for the P1 and ENC1.
    ENC1 is created with fork().

p2.c:
    This file contain the code for the P2 and ENC2.
    ENC2 is created with fork().

channel.c:
    It has the channel process.

shared_memory.c and shared_memory.h:
    This is the files that has all the code for the shared memory.
    And some functions to create the line and decode the line.

semaphore.c and semaphore.h:
    This is the files that has all the code for the semaphores.

util.c and util.h:
    This files has the functions that the threads will execute.
