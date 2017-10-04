#include <stdio.h>
#include <stdlib.h>

#define KB 1024
#define LOOP 1

int main()
{
    int count, *intPtr;
    //clock_t start, end;

    long int i, j, dim = 45 * KB;
    
    //printf("size of an int: %d\n", (int)sizeof(int));

    if ((intPtr = malloc(dim * dim * sizeof(int))) == 0) {
	    perror("totally out of space");
	    exit(1);
    }
    //start = clock();
    for (count = 1; count <= LOOP; count++)
	    for (i = 0; i < dim; i++)
	        for (j = 0; j < dim; j++)
	        {
		        intPtr[i * dim + j] = (i + j) % count;
		        //intPtr[j * dim + i] = (i + j) % count;
		    }
	//end = clock();
	//printf("Loop took %.1f seconds\n", (end-start)/1000000.0);
    free(intPtr);
    return 0;
}
