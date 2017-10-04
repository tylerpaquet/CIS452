#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int pid;
	int numprocesses = 3;
	int procnum, i, status;
	
	for(procnum = 0; procnum < numprocesses; procnum++)
	{
		printf("Spawning child #%d\n", procnum+1);
		pid = fork();
		if(pid == 0)
		{
			break;
		}
	}
	
	if(pid == 0)
	{
		//children
		for(i = 0; i < numprocesses; i++)
		{
			if(procnum == i)
			{
				printf("This is coming from child #%d\n", procnum+1);
				printf("Exiting child #%d\n", procnum+1);
				exit(0);
			}
		}
		
	}
	else
	{
		//parent
		for(i = 0; i < numprocesses; i++)
		{
			wait(&status);
		}
		printf("All children closed.. Exiting program\n");
	}
	return 0;
}
