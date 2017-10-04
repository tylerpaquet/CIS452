/*
* Author:		Tyler Paquet	
* Assignment:	Program 2: Streamed Vector Processing
* Class:		CIS 452
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define MAX 65

void handlerstart(int sig);	//signal handler for ctrl-c to start the program
int waiting = 1;			//flag used to start program from signal handler

int main(int argc, char* argv[])
{
	//start program info
	fprintf(stderr, "--------------------------------------------------------------------------------\n");
	fprintf(stderr, "Hello, this program will take two binary numbers A, and B (from input files) and\n");
	fprintf(stderr, "perform the operation A-B and store the results in a file. Press 'Ctrl-C' when\n");
	fprintf(stderr, "you are ready to begin\n");
	fprintf(stderr, "Starting complementer process..\n");
	
	//pipe file descriptors
	int fd[2];
	int fd2[2];
	
	//data strings
	char comstr[MAX];		//used in complementer
	char incstr[MAX];		//used in incrementer
	char addstr[MAX];		//used in adder
	char addstr2[MAX];		//used in adder
	char readsizestr[MAX];	//used to determine how many bits the input numbers are

	//pid values for forking processes
	pid_t pid1;

	int status;
	
	//file pointers
	FILE *fin1, *fin2, *fout;

	//file info
	int readsize;			//number of bits in each binary input number
	int filelines = 0;		//number of lines in input file
	fin1 = fopen(argv[1], "r");
	fscanf(fin1, "%s", readsizestr);
	readsize = strlen(readsizestr);
	char ch;
	while(!feof(fin1))
	{
	  ch = fgetc(fin1);
	  if(ch == '\n')
	    {
	      filelines++;
	    }
	}
	//filelines++;
	fclose(fin1);

	//open input and output files from command line arguments
	fin1 = fopen(argv[1], "r");
	fin2 = fopen(argv[2], "r");
	fout = fopen(argv[3], "w");

	//open pipes
	pipe(fd);	//pipe1
	pipe(fd2);	//pipe2

	fprintf(stderr, "Spawning incrementer process..\n");

	pid1 = fork();
	if(pid1 == 0)
	{
		fprintf(stderr, "Spawning adder process..\n");
		pid_t pid2;
		pid2 = fork();
		if(pid2 == 0)
		{
			/*******************************************************
			*	          Adder (Grandchild)		       		   *
			********************************************************/

			//signal catch for ctrl-c
			signal(SIGINT, handlerstart);
			while(waiting)
			{
			}
			
			//close unused file descriptors to pipes
			close(fd[READ]);
			close(fd[WRITE]);
			close(fd2[WRITE]);

			while(fscanf(fin2, "%s", addstr2) != EOF) //reads A input from file, line by line
			{

				fprintf(stderr, "(Adder) A-input: %s\n", addstr2);

				read(fd2[READ], (void *) addstr, readsize+1);	//reads string from pipee and stores it in addstr

				fprintf(stderr,"(Adder) Received %s from incrementer\n", addstr);
				fprintf(stderr,"(Adder) Completing expression A-B...\n");

				int k;
				int carry = 0;

				//logic to add two numbers (addstr2 and addstr) together
				for(k = strlen(addstr) - 1; k >= 0; k--)
				{
					if(addstr2[k] == '0' && addstr[k] == '0' && carry == 0)
					{
						addstr[k] = '0';
						carry = 0;
					}
					else if(addstr2[k] == '0' && addstr[k] == '0' && carry == 1)
					{
						addstr[k] = '1';
						carry = 0;
					}
					else if(addstr2[k] == '0' && addstr[k] == '1' && carry == 0)
					{
						addstr[k] = '1';
						carry = 0;
					}
					else if(addstr2[k] == '0' && addstr[k] == '1' && carry == 1)
					{
						addstr[k] = '0';
						carry = 1;
					}
					else if(addstr2[k] == '1' && addstr[k] == '0' && carry == 0)
					{
						addstr[k] = '1';
						carry = 0;
					}
					else if(addstr2[k] == '1' && addstr[k] == '0' && carry == 1)
					{
						addstr[k] = '0';
						carry = 1;
					}
					else if(addstr2[k] == '1' && addstr[k] == '1' && carry == 0)
					{
						addstr[k] = '0';
						carry = 1;
					}
					else if(addstr2[k] == '1' && addstr[k] == '1' && carry == 1)
					{
						addstr[k] = '1';
						carry = 1;
					}

				}
	
				fprintf(stderr, "(Adder) Result: %s\n", addstr);
				
				//write result to file
				fprintf(fout, "%s\n", addstr);
					
			}

			//close files
			fclose(fin2);
			fclose(fout);

			fprintf(stderr, "The program has completed the task. Cleaning up...\n");
			sleep(1);	//just for looks
			fprintf(stderr, "exiting adder..\n");
			sleep(1);	//just for looks
			
		}
		else
		{
			/*******************************************************
			*	          Incrementer (Child)		      		   *
			********************************************************/

			//signal catch for ctrl-c	
			signal(SIGINT, handlerstart);
			while(waiting)
			{
			}

			//close unused file descriptors to pipes
			close(fd[WRITE]);
			close(fd2[READ]);

			int breakflag = 0;
			fprintf(stderr, "---------------------Number of lines in file: %d\n", filelines);
			while(breakflag < filelines)	//only runs as many times as how many numbers are in input file
			{
				//reads from pipe1 and stores value in incstr
				read(fd[READ], (void *) incstr, readsize+1);

				fprintf(stderr, "(Incrementer) Received %s from complementer\n", incstr);
				fprintf(stderr, "(Incrementer) Number is being are being incremented...\n");
				
				int j;
				int carry = 1;

				//adds binary value of 1 to incstr
				for(j = strlen(incstr) - 1; j >= 0; j--)
				{
					if(incstr[j] == '1' && carry == 1)
					{
						incstr[j] = '0';
						carry = 1;
					}
					else if(incstr[j] == '0' && carry == 1)
					{
						incstr[j] = '1';
						carry = 0;
					}
				}
				
				//writes incremented value to pipe 2
				write(fd2[WRITE], (const void *) incstr, (size_t) strlen(incstr) + 1);

				fprintf(stderr, "(Incrementer) Sending %s to adder\n", incstr);
				breakflag++;
			}
			wait(&status);
			fprintf(stderr, "exiting incrementer..\n");
			sleep(1);	//just for looks
		}
	}
	else
	{
		/*******************************************************
		*	          Complementer (Parent)		      		   *
		********************************************************/
		
		//signal catch for ctrl-c
		signal(SIGINT, handlerstart);
		while(waiting)
		{
		}

		//close unused file descriptors to pipes
		close(fd[READ]);
		close(fd2[READ]);
		close(fd2[WRITE]);

		printf("\n"); //leaves ^C on line with Waiting....

		while(fscanf(fin1, "%s", comstr) != EOF)					//reads B input from file, line by line		
		{
			fprintf(stderr, "(Complementer) B-input: %s\n", comstr);
			fprintf(stderr, "(Complementer) Bits are being flipped...\n");
			int i;
			for(i = 0; i < strlen(comstr); i++)				//looks at each number, line by line, and flips bits
			{
				if(comstr[i] == '1')
				{
					comstr[i] = '0';
				}
				else
				{
					comstr[i] = '1';
				}
			}
			
			//writes complemented input B to pipe1
			write(fd[WRITE], (const void *) comstr, (size_t) strlen(comstr) + 1);

			fprintf(stderr, "(Complementer) Sending %s to incrementer\n", comstr);
		}
		fclose(fin1);	//closes B input file
		wait(&status);	//waits for child (complementer) to exit
		fprintf(stderr, "exiting complementer..\n");
		sleep(1);	//just for looks
		fprintf(stderr, "Your results are located in %s\n", argv[3]);
		fprintf(stderr, "Everything closed properly. Goodbye!\n");
		fprintf(stderr, "--------------------------------------------------------------------------------\n");
	}
	return 0;
}

void handlerstart(int sig)
{
	waiting = 0;
}
