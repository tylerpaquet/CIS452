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
void *drawgame(void *args);

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
sem_t print, moles;             //Semaphore names
int score;                      //Keeps track of hit moles
int missed;                     //Keeps track of missed moles
int playgame;                   //Used to exit moles threads when playgame is set to 0

//mole and drawboard thread ids
pthread_t tids[MAXGRID*MAXGRID];
pthread_t did;
	

//##############################################################################################
//                                            MAIN                                             #
//##############################################################################################

int main(int argc, char* argv[])
{
	if(argc < 6)
	{
		fprintf(stderr, "Not enough command line arguments\n");
		fprintf(stderr, "Follow the guidelines below for correct syntax...\n");
		fprintf(stderr, "./a.out rows cols molesup poptime(ms) hidetime(ms)\n");
		exit(1);
	}

	//command line args
	rows = atoi(argv[1]);
	cols = atoi(argv[2]);
	maxmolesup = atoi(argv[3]);
	molemaxpoptime = atoi(argv[4]);
	molemaxhidetime = atoi(argv[5]);
	
	nummoles = rows*cols;
	
	//Checks to make sure moles allowed up doesn't exceed total number of moles
	if(maxmolesup > nummoles)
	{
		fprintf(stderr, "Can't have 'Max Number of Moles Up' greater than 'Number of Moles'\n");
		exit(1);
	}
	
	if(rows > 5 || cols > 5)
	{
		fprintf(stderr, "Rows and Cols must be 5 or less\n");
		exit(1);
	}
	
	if(molemaxpoptime < 1 || molemaxhidetime < 1)
	{
		fprintf(stderr, "Poptime(ms) and Hidetime(ms) must be greater than 1\n");
		exit(1);
	}
	
	playgame = 1;
	score = 0;
	missed = 0;
	
	//random seed
	srand(time(NULL));	
	
	//local vars
	int status, i, j;
	
	//POSIX semaphores
	sem_init(&print, 0, 1);
	sem_init(&moles, 0, maxmolesup);
	
	//initializes board array to 0's
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			board[i][j] = 0;
		}
	}
	
	//initializes moles up array to 0s
	for(i = 0; i < nummoles; i++)
	{
		molesup[i] = 0;
	}
	
	//fills threadnums arr, used to pass to mole threads
	int threadnums[nummoles];
	for(i = 0; i < nummoles; i++)
	{
		threadnums[i] = i;
	}
	
	//creates thread to draw board
    sem_wait(&print); 
	if((status = pthread_create(&did, NULL, drawgame, NULL)) != 0)
	{
		fprintf(stderr, "thread create error %d: %s", status, strerror(status));
		exit(1);
	}
	
	//creates threads to control moles
	for(i = 0; i < nummoles; i++)
	{   
        sem_wait(&print);
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
	
	sem_destroy(&print);
	sem_destroy(&moles);
	endwin();
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
	signal(SIGUSR1, exitprog);
		
	//copies arg to local var
	int threadnum = *((int *) args);
	
	//local vars
	int i, j, k;
	int poptime, hidetime;
	
    sem_post(&print);
	
	
	//calculates random position on board for each mole
	do
	{
		i = rand() % rows;
		j = rand() % cols;
			
	}while(board[i][j] != 0);
	//updates board location to taken
	board[i][j] = 1;
	
	molerow[threadnum] = i;
	molecol[threadnum] = j;
	
	//prints location of every mole
    sem_wait(&print);  
    mvwprintw(mywin, 9, 0, "MOLES:");
	for(k = 0; k < nummoles; k++)
	{
		if(threadnum < 5)
		{
			mvwprintw(mywin, 10+threadnum, 0, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 5 && threadnum < 10)
		{
			mvwprintw(mywin, 5+threadnum, 10, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 10 && threadnum < 15)
		{
			mvwprintw(mywin, threadnum, 20, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 15 && threadnum < 20)
		{
			mvwprintw(mywin, threadnum-5, 30, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
		else if(threadnum >= 20 && threadnum < 25)
		{
			mvwprintw(mywin, threadnum-10, 40, "%c: (%d,%d)", threadnum+65, i+1, j+1);
		}
	}
	refresh();
    sem_post(&print);
	
	//moving mole logic
	while(playgame)
    {
    	//calculates custom pop/hide times based on user input
    	poptime = (rand() % molemaxpoptime) + 250;
		hidetime = (rand() % molemaxhidetime) + 250;
		
    	usleep(hidetime*1000);
    	
    	if(errno == EINTR)
    	{
    		break;
    	}
    	
    	if(!playgame)
    	{
    		break;
    	}
    	
    	sem_wait(&moles);
    	
    	if(!playgame)
    	{
    		sem_post(&moles);
    		break;
    	}
    	
    	sem_wait(&print);
    	
    	if(!playgame)
    	{
    		sem_post(&print);
    		sem_post(&moles);
    		break;
    	}
    		
    	mvwprintw(mywin, i, (j*2)+1, "%c", threadnum+65);
    	refresh();
    	molesup[threadnum] = 1;
    	sem_post(&print);
    	
    	usleep(poptime*1000);
    	
    	if(errno == EINTR)
    	{
    		sem_post(&moles);
    		break;
    	}
    	
    	if(!playgame)
    	{
    		sem_post(&moles);
    		break;
    	}
    	
    	if(molesup[threadnum] == 1 && playgame != 0)
    	{
    		missed++;
    		sem_wait(&print);
    		if(!playgame)
    		{
    			sem_post(&print);
    			sem_post(&moles);
    			break;
    		}
    		mvwprintw(mywin, 17, 7, "%d", missed);
    		sem_post(&print);
    		refresh();
    	}
    	
    	if(!playgame)
    	{
    		sem_post(&moles);
    		break;
    	}
    	
    	sem_wait(&print);
    	
    	if(!playgame)
    	{
    		sem_post(&print);
    		sem_post(&moles);
    		break;
    	}
    	
    	mvwprintw(mywin, i, (j*2)+1, "%c", 0x20);
    	refresh();
    	molesup[threadnum] = 0;
    	sem_post(&print);
    	
    	sem_post(&moles);
    	
    }
	
}

//##############################################################################################
//                                       Draw Game                                             #
//##############################################################################################

void *drawgame(void *args)
{	
	
	//creates window w/no keyboard echo
    mywin = initscr();
    noecho();
    
    //local vars
    int i, j;
    
    
    mvwprintw(mywin, 6, 0, "You chose a %dx%d size board", rows, cols);
    mvwprintw(mywin, 7, 0, "Press the corresponding alpha key to WHACK-A-MOLE or ESC to exit");
    mvwprintw(mywin, 16, 0, "Score:");
    mvwprintw(mywin, 16, 6, "%d", score);
    mvwprintw(mywin, 17, 0, "Missed:");
    mvwprintw(mywin, 17, 7, "%d", missed);
    
    //builds board
    for(i = 0; i < rows; i++)
    {
    	for(j = 0; j <= cols*2; j++)
    	{
    		if(j%2 == 0)
    		{
    			mvwprintw(mywin, i, j, "|");
    		}
    	}
    }
    sem_post(&print);
    
    while(input != 0x1B)
    {
    	input = getch();
    	for(i = 0; i < nummoles; i++)
    	{
    		if((input == i+65 && molesup[i] == 1) || (input == i+97 && molesup[i] == 1))
    		{
    			score++;
    			molesup[i] = 0;
    			sem_wait(&print);
    			mvwprintw(mywin, 16, 6, "%d", score);
    			sem_post(&print);
    			sem_wait(&print);
    			mvwprintw(mywin, molerow[i], (molecol[i]*2)+1, "%c", 0x20);
    			sem_post(&print);
    		}
    	}
    	
    }
    playgame = 0;
    for(i = 0; i < nummoles ; i++)
	{
		pthread_kill(tids[i], SIGUSR1);
	}
}

void exitprog(int sig)
{
	
}




