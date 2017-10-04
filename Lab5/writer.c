/*
* Authors:		Tyler Paquet & Anthony Dowling & JT Kawaski
* Assignment:	Lab 5: Managing Shared Memory
* Class:		CIS 452
* Due:			2/16/2017
*/

/************************
*		WRITER          *
*************************/

#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct Memory {
     int  status;
     char str[30];
};

#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

int  main()
{
     key_t          shmKEY;
     int            shmID;
     struct Memory  *shmPTR;

     shmKEY = ftok(".", 'x');
     shmID = shmget(shmKEY, 100, IPC_CREAT | 0600);
     if (shmID < 0) {
          printf("*** shmget error (writer) ***\n");
          exit(1);
     }
     printf("Writer has received a shared memory segment...\n");

     shmPTR = (struct Memory *) shmat(shmID, NULL, 0);
     if (shmPTR == (void *)-1) {
          printf("*** shmat error (writer) ***\n");
          exit(1);
     }
     printf("Writer has attached the shared memory...\n");

     shmPTR->status  = NOT_READY;
     
     while(1)
     {
     	printf("Enter a string you would like to add to shared memory...\n");
     	scanf("%s", shmPTR->str);
     	
     	if(strcmp(shmPTR->str, "quit") == 0)
     	{
     		shmPTR->status = 5;
     	}
     	
     	else
     	{
     		printf("Writer has put '%s' into shared memory...\n", shmPTR->str);
     		shmPTR->status = FILLED;

     		while (shmPTR->status != TAKEN)
     		{
     			sleep();
     		}
     	}
     
     	if(shmPTR->status == 5)
     	{
     		break;
     	}
     }

     printf("'quit' received...\n");
     shmdt((void *) shmPTR);
     printf("Writer has detached its shared memory...\n");
     shmctl(shmID, IPC_RMID, NULL);
     printf("Writer has removed the shared memory...\n");
     printf("Writer exiting...\n");
     return 0;
}
