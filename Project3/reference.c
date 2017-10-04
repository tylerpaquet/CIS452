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

//Define signal handler
void exitprog(int sig);

//Define thread functions
void *mole(void *args);
void *drawgrid(void *args);

//Global Variables

//mole and drawgrid thread ids

int main(int argc, char* argv[])
{
	//makes sure there enough command line args

	//obtain command line args
	
	//Checks to make sure command line args are acceptable
	
	//initializing global vars
	
	//random seed	
	
	//main local vars
	
	//POSIX semaphores
	
	//initializes board array to 0's
	
	//initializes molesup array to 0s
	
	//fills threadnums arr, passed to mole threads
	
	//creates thread to draw board 
	
	//creates mole threads
	
	//wait for threads to exit
	
	//destroy POSIX semaphores
	
	//close curses window
	
	//print final game statistics
	
	return 0;
}

void *mole(void *args)
{
	//signal catch for endgame
		
	//copies arg to local var
	
	//local vars
	
	//leaves create semaphore
	
	
	//calculates random position on board for each mole
	
	//enter shared memory semaphore
	
	//updates board location to taken
	
	//leaves shared memory semaphore
	
	//puts (row, col) coordinates into correct arrays
	
	//moving mole logic
	while(playgame)
    {
    	//calculates custom pop/hide times based on user input
		
		//sleeps for the calculated hidetime
    	
    	//exit when sleep is interupted by signal
    	
    	//exit if !playgame
    	
    	//enter moles semaphore
    	
    	//exit if !playgame
    	
    	//enter print semaphore
    	
    	//exit if !playgame
    	
    	//make mole visible
    	
    	//update molesup array, used to detect 'hit' in draw thread
    	
    	//leave print semaphore
    	
    	//sleeps for calculated poptime
    	
    	//exit when sleep is interupted by signal
    	
    	//exit if !playgame
    	
    	//detects missed mole (never was hit while visible)
    	
    	//exit if !playgame
    	
    	//enter print semaphore
    	
    	//exit if !playgame
    	
    	//makes mole disappear
    	
    	//update molesup array, used to detect 'hit' in draw thread
    	
    	//leave print semaphore
    	
    	//leave moles semaphore
    	
    }
	
}

void *drawgrid(void *args)
{	
	
	//creates window w/no keyboard echo
    
    //local vars
    
    //enter print semaphore
    
    //prints game info
    
    //leave print semaphore
    
    //builds grid

    //leave create semaphore
    
    
   	//game loop, exit upon ESC input
   	while(input != 0x1B)
   	{
   		//keyboard input
		
		//checks for 'hit' moles
    	
    }
    
    //end game when ESC is recieved from keyboard
    
    //send user interrupt to mole threads to wake up from sleep

}

//void function used for signal handler (does nothing)
void exitprog(int sig)
{	
}

