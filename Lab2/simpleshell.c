#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>


#define MAXLINE 80
#define MAXARG  20

void childprocess (char * cmd);

int main()
{
    char cmd[MAXLINE];
    struct rusage r_usage;

    for(; ;)
    {
        printf("SimpleShell$ ");
        fgets(cmd, MAXLINE, stdin);

        if(strcmp(cmd, "exit\n") == 0)
        {
            exit(0);
        }
        else
        {
            int i = fork();
            if(i == 0)
            {
                childprocess(cmd);
            }
        }
	
	wait3(0,0, &r_usage);
	printf("User CPU time used: %ld.%06ld sec\n", r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec);
	printf("Number of involuntary context switches: %ld\n", r_usage.ru_nivcsw);
    }
}

void childprocess (char * cmd)
{
    int i = 0;
    char *argv[MAXARG];
    char *token;
    token = strtok(cmd," \n");

    while(token != NULL)
    {
        argv[i] = token;
        token = strtok(NULL," \n");
        i++;
    }
    
    argv[i] = NULL;

    execvp(argv[0], &argv[0]);
    printf("Unknown command\n");
}

