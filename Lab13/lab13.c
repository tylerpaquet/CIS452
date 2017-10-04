#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	
	char OGfile[100];
	char linkFile[100];
	int selector = 0;
	
	printf("\n");
	printf("This program creates a hard/symbolic link to a file of your choosing\n");
	printf("Please enter the file you would like to create a link to\n");
	scanf("%s", OGfile);
	printf("Please enter a name for the link\n");
	scanf("%s", linkFile);
	
	while(1)
	{
		printf("Would you like to create a Hard Hink (1) or a Symbolic Link? (2)\n");
		scanf("%d", &selector);
		
		if(selector == 1 || selector == 2)
		{
			break;
		}
	}
	
	if(selector == 1)
	{
		printf("You chose to create a hard link\n");
		if(link(OGfile, linkFile) != 0)
		{
			printf("Error creating link\n");
			exit(1);
		}
		printf("Hard link created\n");
	}
	
	else
	{
		printf("You chose to create a symbolic link\n");
		if(symlink(OGfile, linkFile) != 0)
		{
			printf("Error creating link\n");
			exit(1);
		}
		printf("Symbolic link created\n");
		
	}
	
	return 0;
}
