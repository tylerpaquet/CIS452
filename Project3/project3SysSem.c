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

#define MAXGRID 5

void *mole(void *args);
void *drawgame(void *args);
int readymoles = 0;
int numthreads;
int board[MAXGRID][MAXGRID]; //value is 1 if spot on board (index) is taken, 0 if available
//add cols array
int rows, cols;
char input;
int semID, semID2;
int maxmolesup, molemaxpoptime, molemaxhidetime;
WINDOW *mywin;












int main()
{
	//random seed
	srand(time(NULL));	
	
	//local vars
	int status, i, j;
	
	//will be changed to command line args
	rows = 5;
	cols = 3;
	numthreads = rows*cols;
	maxmolesup = 2;
	molemaxpoptime = 1000000;
	molemaxhidetime = 1000000;
	
	//semaphore size 1
	struct sembuf semBUF;
	semID = semget(IPC_PRIVATE, 1, 00600);
	semctl(semID, 0, SETVAL, 1);
	
	//semaphore size maxmolesup
	struct sembuf semBUF2;
	semID2 = semget(IPC_PRIVATE, 1, 00600);
	semctl(semID2, 0, SETVAL, maxmolesup);
	
	//mole and drawboard thread ids
	pthread_t tids[numthreads];
	pthread_t did;
	
	//initializes board array to 0's
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			board[i][j] = 0;
		}
	}
	
	//fills threadnums arr, used to pass to mole threads
	int threadnums[numthreads];
	for(i = 0; i < numthreads; i++)
	{
		threadnums[i] = i;
	}
	
	//creates thread to draw board
	semBUF.sem_num = 0;
    semBUF.sem_op = -1;
    semBUF.sem_flg = 0;
    semop(semID, &semBUF, 1);
	if((status = pthread_create(&did, NULL, drawgame, NULL)) != 0)
	{
		fprintf(stderr, "thread create error %d: %s", status, strerror(status));
		exit(1);
	}
	
	//creates threads to control moles
	for(i = 0; i < numthreads; i++)
	{
		semBUF.sem_num = 0;
        semBUF.sem_op = -1;
        semBUF.sem_flg = 0;
        semop(semID, &semBUF, 1);
		if((status = pthread_create(&tids[i], NULL, mole, &threadnums[i])) != 0)
		{
			fprintf(stderr, "thread create error %d: %s", status, strerror(status));
			exit(1);
		}
	}
	
	//waits for threads to exit
	for(i = 0; i < numthreads; i++)
	{
		pthread_join(tids[i], NULL);
	}
	pthread_join(did, NULL);
	
	//removes semaphores
	semctl(semID, 0, IPC_RMID);
	semctl(semID2, 0, IPC_RMID);
	return 0;
}













void *mole(void *args)
{
	//copies arg to local var
	int threadnum = *((int *) args);
	
	//local vars
	int i, j, k;
	int poptime, hidetime;
	struct sembuf semBUF;
	struct sembuf semBUF2;
	
	semBUF.sem_num = 0;
	semBUF.sem_op = 1;
    semop(semID, &semBUF, 1);
	
	
	//calculates random position on board for each mole
	do
	{
		i = rand() % rows;
		j = rand() % cols;
			
	}while(board[i][j] != 0);
	//updates board location to taken
	board[i][j] = 1;
	
	//calculates custom pop/hide times based on user input
	poptime = rand() % molemaxpoptime + 1000000;
	hidetime = rand() % molemaxhidetime + 1000000;
	
	
	//prints location of every mole
	semBUF.sem_num = 0;
    semBUF.sem_op = -1;
    semBUF.sem_flg = 0;
    semop(semID, &semBUF, 1);
    
    mvwprintw(mywin, 9, 0, "MOLES:");
	for(k = 0; k < numthreads; k++)
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
	readymoles++;
	
	semBUF.sem_num = 0;
	semBUF.sem_op = 1;
    semop(semID, &semBUF, 1);
	
	//moving mole logic
	while(input != 0x1B)
    {
    	semBUF2.sem_num = 0;
    	semBUF2.sem_op = -1;
    	semBUF2.sem_flg = 0;
    	semop(semID2, &semBUF2, 1);
    	
    	
    	semBUF.sem_num = 1;
    	semBUF.sem_op = -1;
    	semBUF.sem_flg = 0;
    	semop(semID, &semBUF, 1);
    	mvwprintw(mywin, i, (j*2)+1, "%c", threadnum+65);
    	refresh();
    	semBUF.sem_num = 1;
		semBUF.sem_op = 1;
    	semop(semID, &semBUF, 1);
    	
    	usleep(poptime);
    	
    	semBUF.sem_num = 1;
    	semBUF.sem_op = -1;
    	semBUF.sem_flg = 0;
    	semop(semID, &semBUF, 1);
    	mvwprintw(mywin, i, (j*2)+1, "%c", 0x20);
    	refresh();
    	semBUF.sem_num = 1;
		semBUF.sem_op = 1;
    	semop(semID, &semBUF, 1);
    	
    	
    	semBUF2.sem_num = 0;
		semBUF2.sem_op = 1;
    	semop(semID2, &semBUF2, 1);
    	usleep(hidetime);
    	
    }
	
}













void *drawgame(void *args)
{
	struct sembuf semBUF;
	
	//creates window w/no keyboard echo
    mywin = initscr();
    noecho();
    
    //local vars
    int i, j;
    
    
    mvwprintw(mywin, rows+1, 0, "You chose a %dx%d size board", rows, cols);
    
    mvwprintw(mywin, rows+2, 0, "Type a character or ESC");
    
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
    semBUF.sem_num = 0;
	semBUF.sem_op = 1;
    semop(semID, &semBUF, 1);
    
    while(input != 0x1B)
    {
    	input = getch();
    }
    
	endwin();
}




