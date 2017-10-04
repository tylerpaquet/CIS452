/*
*Author:		Tyler Paquet
*Assignment:	Program 3: Whack-a-Mole
*Class:			CIS 452
*Due:			04/06/2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>
#include <error.h>

#define MAXGRID 5

//Define signal handler
void exitprog(int sig);

//Define thread functions
void *mole(void *args);
void *drawgrid(void *args);

//Global Variables
int nummoles;                   //holds number of mole threads
int board[MAXGRID][MAXGRID];    //value is 1 if spot on board (index) is taken, 0 if available
int molesup[MAXGRID*MAXGRID];   //holds a 1 if mole is up
int molerow[MAXGRID*MAXGRID];   //hold row location of each mole
int molecol[MAXGRID*MAXGRID];   //hold col location of each mole
int rows, cols;                 //holds rows and cols for game board
char input;                     //holds keyboard input
int maxmolesup;                 //holds max moles allowed to be visible
int molemaxpoptime;             //holds max time moles can be visible
int molemaxhidetime;            //holds max time moles can hide for
WINDOW *mywin;                  //Window for curses functions
sem_t print, moles, create;     //Semaphore names
sem_t sharedmem;
int score;                      //Keeps track of hit moles
int missed;                     //Keeps track of missed moles
int playgame;                   //Used to exit moles threads when playgame is set to 0

//mole and drawgrid thread ids
pthread_t tids[MAXGRID*MAXGRID];
pthread_t did;
	
//##############################################################################################
//                                            MAIN                                             #
//##############################################################################################

int main(int argc, char* argv[])
{
	//makes sure there enough command line args
	if(argc < 6)
	{
		fprintf(stderr, "\nNot enough command line arguments...\n");
		fprintf(stderr, "Follow the guidelines below for correct syntax...\n");
		fprintf(stderr, "--------------------------------------------------------\n");
		fprintf(stderr, "./project3 rows cols maxmolesup poptime(ms) hidetime(ms)\n");
		fprintf(stderr, "--------------------------------------------------------\n\n");
		fprintf(stderr, "Example using a 5x5 board, 10 moles allowed up at once,\n");
		fprintf(stderr, "max pop time and max hide time set to 1 second\n");
		fprintf(stderr, "--------------------------------------------------------\n");
		fprintf(stderr, "./project3 5 5 10 1000 1000\n");
		fprintf(stderr, "--------------------------------------------------------\n\n");
		exit(1);
	}

	//command line args
	rows = atoi(argv[1]);
	cols = atoi(argv[2]);
	maxmolesup = atoi(argv[3]);
	molemaxpoptime = atoi(argv[4]);
	molemaxhidetime = atoi(argv[5]);
	
	nummoles = rows*cols;
	
	//Checks to make sure command line args are acceptable
	if(maxmolesup > nummoles)
	{
		fprintf(stderr, "\nCan't have 'maxmolesup' greater than 'rows*cols'\n\n");
		exit(1);
	}
	if(rows > 5 || cols > 5)
	{
		fprintf(stderr, "\nRows and Cols must be 5 or less\n\n");
		exit(1);
	}
	if(rows < 1 || cols < 1)
	{
		fprintf(stderr, "\nRows and Cols must be 1 or greater\n\n");
		exit(1);
	}
	if(molemaxpoptime < 1 || molemaxhidetime < 1)
	{
		fprintf(stderr, "\nPoptime(ms) and Hidetime(ms) must be greater than 0\n\n");
		exit(1);
	}
	
	//initializing global vars
	playgame = 1;
	score = 0;
	missed = 0;
	
	//random seed
	srand(time(NULL));	
	
	//main local vars
	int status, i, j;
	
	//POSIX semaphores
	sem_init(&print, 0, 1);
	sem_init(&moles, 0, maxmolesup);
	sem_init(&create, 0, 1);
	sem_init(&sharedmem, 0, 1);
	
	//initializes board array to 0's
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			board[i][j] = 0;
		}
	}
	
	//initializes molesup array to 0s
	for(i = 0; i < nummoles; i++)
	{
		molesup[i] = 0;
	}
	
	//fills threadnums arr, passed to mole threads
	int threadnums[nummoles];
	for(i = 0; i < nummoles; i++)
	{
		threadnums[i] = i;
	}
	
	//creates thread to draw board
    sem_wait(&create); 
	if((status = pthread_create(&did, NULL, drawgrid, NULL)) != 0)
	{
		fprintf(stderr, "thread create error %d: %s", status, strerror(status));
		exit(1);
	}
	
	//creates mole threads
	for(i = 0; i < nummoles; i++)
	{   
        sem_wait(&create);
		if((status = pthread_create(&tids[i], NULL, mole, &threadnums[i])) != 0)
		{
			fprintf(stderr, "thread create error %d: %s", status, strerror(status));
			exit(1);
		}
	}
	
	//waits for threads to exit
	for(i = 0; i < nummoles; i++)
	{
		pthread_join(tids[i], NULL);
	}
	pthread_join(did, NULL);
	
	//destroy POSIX semaphores
	sem_destroy(&print);
	sem_destroy(&moles);
	sem_destroy(&create);
	sem_destroy(&sharedmem);
	
	//close curses window
	endwin();
	
	//print final game statistics
	printf("\n");
	printf("GAMEOVER\n");
	printf("You hit %d moles!\n", score);
	printf("You missed %d moles!\n", missed);
	if(score >= missed)
	{
		printf("You're pretty good!\n");
	}
	else
	{
		printf("You can do better..\n");
	}
	printf("\n");
	
	return 0;
}

//##############################################################################################
//                                             MOLES                                           #
//##############################################################################################

void *mole(void *args)
{
	//signal catch for endgame
	signal(SIGUSR1, exitprog);
		
	//copies arg to local var
	int threadnum = *((int *) args);
	
	//local vars
	int i, j, k;
	int poptime, hidetime;
	
	//leaves create semaphore
    sem_post(&create);
	
	
	//calculates random position on board for each mole
	sem_wait(&sharedmem);
	do
	{
		i = rand() % rows;
		j = rand() % cols;
			
	}while(board[i][j] != 0);
	
	//updates board location to taken
	board[i][j] = 1;
	sem_post(&sharedmem);
	
	//puts (row, col) coordinates into correct arrays
	molerow[threadnum] = i;
	molecol[threadnum] = j;
	
	sem_wait(&print);
	mvwprintw(mywin, 19, 0, "MOLE LOCATIONS:");
	for(k = 0; k < nummoles; k++)
	{
		if(threadnum < 5)
		{
			mvwprintw(mywin, 20+threadnum, 0, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 5 && threadnum < 10)
		{
			mvwprintw(mywin, 15+threadnum, 10, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 10 && threadnum < 15)
		{
			mvwprintw(mywin, 10+threadnum, 20, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 15 && threadnum < 20)
		{
			mvwprintw(mywin, 5+threadnum, 30, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 20 && threadnum < 25)
		{
			mvwprintw(mywin, threadnum, 40, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
	}
	sem_post(&print);
	
	//moving mole logic
	while(playgame)
    {
    	//calculates custom pop/hide times based on user input
    	poptime = (rand() % molemaxpoptime) /*+ 250*/;
		hidetime = (rand() % molemaxhidetime) /*+ 250*/;
		
		//sleeps for the calculated hidetime
    	usleep(hidetime*1000);
    	
    	//exit when sleep is interupted by signal
    	if(errno == EINTR)
    	{
    		break;
    	}
    	
    	//exit
    	if(!playgame)
    	{
    		break;
    	}
    	
    	//enter moles semaphore
    	sem_wait(&moles);
    	
    	//exit
    	if(!playgame)
    	{
    		//leave semaphores before exit
    		sem_post(&moles);
    		break;
    	}
    	
    	//enter print semaphore
    	sem_wait(&print);
    	
    	//exit
    	if(!playgame)
    	{
    		//leave semaphores before exit
    		sem_post(&print);
    		sem_post(&moles);
    		break;
    	}
    	
    	//makes mole visible	
    	mvwprintw(mywin, (i*3)+1, (j*7)+3, "%c", threadnum+97);
    	mvwprintw(mywin, (i*3)+1, (j*7)+4, "%c", threadnum+97);
    	mvwprintw(mywin, (i*3)+2, (j*7)+2, "%c", threadnum+97);
    	mvwprintw(mywin, (i*3)+2, (j*7)+3, "%c", threadnum+97);
    	mvwprintw(mywin, (i*3)+2, (j*7)+4, "%c", threadnum+97);
    	mvwprintw(mywin, (i*3)+2, (j*7)+5, "%c", threadnum+97);
    	refresh();
    	
    	//update molesup array, used to detect 'hit' in draw thread
    	molesup[threadnum] = 1;
    	
    	//leave print semaphore
    	sem_post(&print);
    	
    	//sleeps for calculated poptime
    	usleep(poptime*1000);
    	
    	//exit when sleep is interupted by signal
    	if(errno == EINTR)
    	{
    		//leave semaphores before exit
    		sem_post(&moles);
    		break;
    	}
    	
    	//exit
    	if(!playgame)
    	{
    		//leave semaphores before exit
    		sem_post(&moles);
    		break;
    	}
    	
    	//detects missed mole (never was hit while visible)
    	if(molesup[threadnum] == 1 && playgame != 0)
    	{
    		//updates missed count
    		missed++;
    		
    		//enter print semaphore
    		sem_wait(&print);
    		
    		//exit
    		if(!playgame)
    		{
    			//leave semaphores before exit
    			sem_post(&print);
    			sem_post(&moles);
    			break;
    		}
    		
    		//updates missed count to board
    		mvwprintw(mywin, 3, (cols*7) + 10, "%d", missed);
    		refresh();
    		
    		//leaves print semaphore
    		sem_post(&print);
    		
    	}
    	
    	//exit
    	if(!playgame)
    	{
    		//leave semaphores before exit
    		sem_post(&moles);
    		break;
    	}
    	
    	//enter print semaphore
    	sem_wait(&print);
    	
    	//exit
    	if(!playgame)
    	{
    		//leave semaphores before exit
    		sem_post(&print);
    		sem_post(&moles);
    		break;
    	}
    	
    	//makes mole disappear
    	mvwprintw(mywin, (i*3)+1, (j*7)+3, "%c", 0x20);
    	mvwprintw(mywin, (i*3)+1, (j*7)+4, "%c", 0x20);
    	mvwprintw(mywin, (i*3)+2, (j*7)+2, "%c", 0x20);
    	mvwprintw(mywin, (i*3)+2, (j*7)+3, "%c", 0x20);
    	mvwprintw(mywin, (i*3)+2, (j*7)+4, "%c", 0x20);
    	mvwprintw(mywin, (i*3)+2, (j*7)+5, "%c", 0x20);
    	refresh();
    	
    	//update molesup array, used to detect 'hit' in draw thread
    	molesup[threadnum] = 0;
    	
    	//leave print semaphore
    	sem_post(&print);
    	
    	//leave moles semaphore
    	sem_post(&moles);
    	
    }
	
}

//##############################################################################################
//                                       Draw Game                                             #
//##############################################################################################

void *drawgrid(void *args)
{	
	
	//creates window w/no keyboard echo
    mywin = initscr();
    noecho();
    
    //local vars
    int i, j;
    
    //enter print semaphore
    sem_wait(&print);
    
    //prints game info
    if(rows > 2)
    {
    	mvwprintw(mywin, (rows*3)+2, 0, "You chose a %dx%d size board", rows, cols);
    	mvwprintw(mywin, (rows*3)+3, 0, "Press the corresponding alpha key to WHACK-A-MOLE or ESC to exit");
    }
    else
    {
    	mvwprintw(mywin, 10, 0, "You chose a %dx%d size board", rows, cols);
    	mvwprintw(mywin, 11, 0, "Press the corresponding alpha key to WHACK-A-MOLE or ESC to exit");
    }
    
    mvwprintw(mywin, 1, (cols*7) + 2, "Score:");
    mvwprintw(mywin, 1, (cols*7) + 9, "%d", score);
    mvwprintw(mywin, 3, (cols*7) + 2, "Missed:");
    mvwprintw(mywin, 3, (cols*7) + 10, "%d", missed);
    mvwprintw(mywin, 5, (cols*7) + 2, "Max Moles Allowed Up:");
    mvwprintw(mywin, 5, (cols*7) + 24, "%d", maxmolesup);
    mvwprintw(mywin, 7, (cols*7) + 2, "Max Pop Time:");
    mvwprintw(mywin, 7, (cols*7) + 16, "%d milliseconds", molemaxpoptime);
    mvwprintw(mywin, 9, (cols*7) + 2, "Max Hide Time:");
    mvwprintw(mywin, 9, (cols*7) + 17, "%d milliseconds", molemaxhidetime);
    refresh();
    
    //leave print semaphore
    sem_post(&print);
    
    //builds grid
    for(i = 0; i < (rows*3) + 1; i++)
    {
    	for(j = 0; j < (cols*7) + 1; j++)
    	{
    		if(i%3 == 0)
    		{
    			//enter print semaphore
    			sem_wait(&print);
    			
    			mvwprintw(mywin, i, j, "-");
    			
    			//leave print semaphore
    			sem_post(&print);
    		}
    		if(j%7 == 0)
    		{
    			//enter print semaphore
    			sem_wait(&print);
    			
    			mvwprintw(mywin, i, j, "|");
    			
    			//leave print semaphore
    			sem_post(&print);
    		}
    	}
    }
    //leave create semaphore
    sem_post(&create);
    
    
   	//game loop, exit upon ESC input
   	while(input != 0x1B)
   	{
   		//keyboard input
		input = getch();
		
		//checks for 'hit' moles
    	for(i = 0; i < nummoles; i++)
    	{
    		if((input == i+65 && molesup[i] == 1) || (input == i+97 && molesup[i] == 1))
    		{
    			//update score if mole is hit
    			score++;
    			
    			//update molesup array
    			molesup[i] = 0;
    			
    			//enter print semaphore
    			sem_wait(&print);
    			
    			//update score to board
    			mvwprintw(mywin, 1, (cols*7) + 9, "%d", score);
    			refresh();
    			
    			//leave print semaphore
    			sem_post(&print);
    			
    			//enter print semaphore
    			sem_wait(&print);
    			
    			//makes mole disappear after it's hit
    			mvwprintw(mywin, (molerow[i]*3)+1, (molecol[i]*7)+3, "%c", 0x20);
    			mvwprintw(mywin, (molerow[i]*3)+1, (molecol[i]*7)+4, "%c", 0x20);
    			mvwprintw(mywin, (molerow[i]*3)+2, (molecol[i]*7)+2, "%c", 0x20);
    			mvwprintw(mywin, (molerow[i]*3)+2, (molecol[i]*7)+3, "%c", 0x20);
    			mvwprintw(mywin, (molerow[i]*3)+2, (molecol[i]*7)+4, "%c", 0x20);
    			mvwprintw(mywin, (molerow[i]*3)+2, (molecol[i]*7)+5, "%c", 0x20);
    			refresh();
    			
    			//leave print semaphore
    			sem_post(&print);
    		}
    	}
    	
    }
    
    //end game when ESC is recieved from keyboard
    playgame = 0;
    
    //send user interrupt to mole threads to wake up from sleep
    for(i = 0; i < nummoles ; i++)
	{
		pthread_kill(tids[i], SIGUSR1);
	}
}

//void function used for signal handler (does nothing)
void exitprog(int sig)
{	
}



