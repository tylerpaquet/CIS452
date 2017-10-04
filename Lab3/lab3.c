/*
* Lab 3
* Tyler Paquet
* Anthony Dowling
* Due: 2/2/2017
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

void handler1(int sig);
void handler2(int sig);
void handler3(int sig);
void handler4(int sig);

int exitprog = 0;

int main()
{
 	pid_t pid1;
 	signal(SIGUSR1, handler1);
 	signal(SIGUSR2, handler2);
 	pid1 = fork();
 	if(pid1 == 0)
	{
		signal(SIGINT, handler4); 
		while(pid1 == 0)
		{
			srand(time(NULL));
			int randtime = (rand() % 5) + 1;
 			sleep(randtime); 			
			int randsig = (rand() % 2) + 1;

			if(randsig == 1)
 			{
 				kill(getppid(), SIGUSR1);
			} 
 			if(randsig == 2)
 			{
 				kill(getppid(), SIGUSR2);
 			}
 		}
	}
 	else
 	{
 		signal(SIGINT, handler3);
 		printf("spawned child PID# %d\n", pid1);
 		while(exitprog == 0)
 		{
 			printf("waiting... ");
 			fflush(stdout);
 			pause();
 		}
 		printf("That's it, I'm shutting you down...\n");
 		kill(pid1, SIGINT);
		wait();
 	}
 	return 0; 
}

void handler1(int sig)
{
 	printf("received a SIGUSR1 signal\n");
}

void handler2(int sig)
{ 
 	printf("received a SIGUSR2 signal\n");
}

void handler3(int sig)
{
 	printf(" received.\n");
 	exitprog = 1; 
} 

void handler4(int sig)
{
	exit(0); 
} 
