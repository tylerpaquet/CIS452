#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
    time_t start, end;
    double diff;
    int milliseconds;
    
    printf("How many milliseconds would you like to sleep for?");
    scanf("%d", &milliseconds);
    
    printf("sleeping....\n");
    time(&start);
    usleep(1000*milliseconds);
    time(&end);
    printf("done sleeping\n");
    diff = difftime(end, start);
    
    printf("Execution time = %.0f milliseconds\n", diff*1000);

    return 0;
}
