#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	
	unsigned int randval;
   	FILE *f; 
    f = fopen("/dev/random", "r");
    fread(&randval, sizeof(randval), 1, f);
    fclose(f);
    
    srandom(randval);
    
    int i;
    long int randnum;
    for(i = 0; i < 10; i++)
    {
    	randnum = random();
    	printf("Random number %d: %d\n", i+1, randnum);
    }
		
	
	return 0;
}

/*Sample output
[eos26:~/Desktop/CIS452/Lab14]$ ./lab14
Random number 1: 1769609763
Random number 2: 1663585150
Random number 3: 1116876865
Random number 4: 658686198
Random number 5: 373262470
Random number 6: 2079176783
Random number 7: 1228172937
Random number 8: 1292859526
Random number 9: 601574213
Random number 10: 1619396395*/

