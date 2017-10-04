#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct stat statBuf;
    DIR *dirPtr;
    struct dirent *entryPtr;
    char str[100];

    if (argc < 2) {
	printf("Usage: directory required\n");
	exit(1);
    }

    if (stat(argv[1], &statBuf) < 0) {
	perror("bruh..  there is ");
	exit(1);
    }

	if(statBuf.st_mode == 33152)
	{
		printf("%s is not a directory\n", argv[1]);
		exit(1);
	}
	else
	{
		printf("\n");
		printf("File Name\t\t|\tFile Size\t|\tInode Number\t|\n");
		printf("-------------------------------------------------------------------------\n");
		dirPtr = opendir(argv[1]);

    	while ((entryPtr = readdir(dirPtr)))
    	{
    		strcpy(str, argv[1]);
    		strcat(str, "/");
    		strcat(str, (char *)entryPtr->d_name);
    		stat(str, &statBuf);
    		if(strcmp((char *)entryPtr->d_name, ".") == 0 || strcmp((char *)entryPtr->d_name, "..") == 0)
    		{
    		}
    		else
    		{
    			if(statBuf.st_size < 100)
    			{
    				printf("%-20s\t|\t%u Bytes\t\t|\t%u\t|\n", entryPtr->d_name, statBuf.st_size, entryPtr->d_ino);
    			}
    			else
    			{
    				printf("%-20s\t|\t%u Bytes\t|\t%u\t|\n", entryPtr->d_name, statBuf.st_size, entryPtr->d_ino);	
    			}
    		}
    		
    	}
    	closedir(dirPtr);
    	printf("\n");
		}

    
    return 0;
}
