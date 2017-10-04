/*
* Authors:		Tyler Paquet & Anthony Dowling & JT Kawaski
* Assignment:	Lab 5: Managing Shared Memory
* Class:		CIS 452
* Due:			2/16/2017
*/

/************************
*		READER          *
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
     shmID = shmget(shmKEY, 100, 0600);
     if (shmID < 0) {
          printf("*** shmget error (reader) ***\n");
          exit(1);
     }
     printf("Reader has received a shared memory segment...\n");

     shmPTR = (struct Memory *) shmat(shmID, NULL, 0);
     if (shmPTR == (void *)-1) {
          printf("*** shmat error (reader) ***\n");
          exit(1);
     }
     printf("Reader has attached the shared memory...\n");
     
     while(1)
     {
     	while (shmPTR->status != FILLED)
     	{
     		if(strcmp(shmPTR->str, "quit") == 0)
     		{
     			break;
     		}
     	}
     	if(strcmp(shmPTR->str, "quit") == 0)
     	{
     		break;
     	}
     	printf("Reader found '%s' in shared memory...\n", shmPTR->str);

     	shmPTR->status = TAKEN;
     	
     }
     
     printf("'quit' received...\n");
     shmdt((void *) shmPTR);
     printf("Reader has detached its shared memory...\n");
     printf("Reader exiting...\n");
     return 0;
}
