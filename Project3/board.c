/* To compile:
   gcc this-file.c   -lcurses 
 */
#include <curses.h>
#include <stdio.h>

int main() {
	int dim1, dim2;
	printf("Board will be size AxB\n");
	printf("Enter size for A ");
	scanf("%d", &dim1);
	printf("Enter size for B ");
	scanf("%d", &dim2);
    initscr();
    noecho();    /* do not echo keyboard input */
    int i, j;
    
    char input;
    move(dim1, 0);
    printw("You chose a %dx%d size board", dim1, dim2);
    move(dim1+1, 0);
    printw("Type a character or ESC");
    
    //build board
    for(i = 0; i < dim1; i++)
    {
    	for(j = 0; j <= dim2*2; j++)
    	{
    		if(j%2 == 0)
    		{
    			move(i,j);
    			printw("|");
    		}
    	}
    }
    
    //print to board
    input = getch();
    while (input != 0x1B) //loop until esc
    {   
        for(i = 0; i < dim1; i++)
        {
        	for(j = 0; j <= dim2*2; j++)
        	{
        		if(j%2 == 1)
        		{
        			move(i, j);
        			printw("%c", input);
        			input = getch();
        			if(input == 0x1B)
        			{
        				break;
        			}
        		}
        		if(input == 0x1B)
        		{
        			break;
        		}
        	}
        }
        //refresh();    // when manual refresh is required
    }
    endwin();
    return 0;
}
