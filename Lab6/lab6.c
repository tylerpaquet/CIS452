/*
* Authors:		Tyler Paquet & Anthony Dowling & JT Kawaski
* Assignment:	Lab 6: Shared Memory Synchronization
* Class:		CIS 452
* Due:			2/23/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

int main (int argc, char* argv[]) {
    int status;
    long int i, loop, temp, *shmPtr;
    int shmId, semID;
    pid_t pid;
    struct sembuf semBUF;
     
    sscanf(argv[1], "%ld", &loop);
    printf("%ld\n", loop);
     
    if ((shmId = shmget (IPC_PRIVATE, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
    {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1)
    {
        perror ("can't attach\n");
        exit (1);
    }

    shmPtr[0] = 0;
    shmPtr[1] = 1;
    
    semID = semget(IPC_PRIVATE, 1, 00600);
    semctl(semID, 0, SETVAL, 1);

    if (!(pid = fork ()))
    {
    	
        for (i = 0; i < loop; i++)
        {
        	 semBUF.sem_num = 0;
        	 semBUF.sem_op = -1;
        	 semBUF.sem_flg = 0;
        	 semop(semID, &semBUF, 1);
             temp = shmPtr[0];
             shmPtr[0] = shmPtr[1];
             shmPtr[1] = temp;
             semBUF.sem_op = 1;
             semop(semID, &semBUF, 1);
        }
        if (shmdt (shmPtr) < 0)
        {
            perror ("just can 't let go\n");
            exit (1);
        }
        exit (0);
    }
    else
    {
    	
        for (i = 0; i < loop; i++)
        {
        	 semBUF.sem_num = 0;
        	 semBUF.sem_op = -1;
        	 semBUF.sem_flg = 0;
        	 semop(semID, &semBUF, 1);
             temp = shmPtr[1];
             shmPtr[1] = shmPtr[0];
             shmPtr[0] = temp;
             semBUF.sem_op = 1;
             semop(semID, &semBUF, 1);
        }
    }

    wait (&status);
    printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
    
    semctl(semID, 0, IPC_RMID);
    return 0;
}
