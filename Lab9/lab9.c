/*
*Tyler Paquet
*Anthony Dowling
*JT Kawaski
*/
#include <stdio.h>
#include <stdbool.h>

double globalbig[500];
double globalsmall[50];
int globalinit[] = {1,2,3,4,5};

int main() {
    bool bool1, bool2;
    int int1, int2;
    int int3 = 1;
    float float1, float2;
    float float3 = 1.0;
    float localbig[500];
    float localsmall[50];
    int localinit[] = {1,2,3,4,5};
    float *p1, *p2;
    int *p3, *p4, *p5, *p6;
    double *p7, *p8;
    int *mallp1;
    float *mallp2;
    double *mallp3;
    bool *mallp4;
    p1 = localbig;
    p2 = localsmall;
    p3 = globalinit;
    p5 = localinit;
    p7 = globalbig;
    p8 = globalsmall;
    mallp1 = (int *)malloc(sizeof(int));
    mallp2 = (float *)malloc(sizeof(float));
    mallp3 = (double *)malloc(sizeof(double));
    mallp4 = (bool *)malloc(sizeof(bool));
	
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("This main function begins at \t\t\t\t%X\n", main);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two global arrays are allocated at \t\t\t%X and %X\n", globalbig, globalsmall);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The global initialized array is allocated at \t\t%X\n", globalinit);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two local uninitialized ints are allocated at \t%X and %X\n", &int1, &int2);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local initialized int is allocated at \t\t%X\n", &int3);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two local uninitialized floats are allocated at \t%X and %X\n", &float1, &float2);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local initialized float is allocated at \t\t%X\n", &float3);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two local arrays are allocated at \t\t\t%X and %X\n", localbig, localsmall);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local initialized array is allocated at \t\t%X\n", localinit);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two local float pointers are allocated at \t\t%X and %X\n", &p1, &p2);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The four local int pointers are allocated at \t\t%X and %X and %X and %X\n", &p3, &p4, &p5, &p6);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The two local double pointers are allocated at \t\t%X and %X\n", &p7, &p8);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local int malloc is allocated at \t\t\t%X\n", mallp1);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local float malloc is allocated at \t\t\t%X\n", mallp2);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local int malloc is allocated at \t\t\t%X\n", mallp3);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    printf ("The local float malloc is allocated at \t\t\t%X\n", mallp4);
    printf ("--------------------------------------------------------------------------------------------------------\n");
    
    free(mallp1);
    free(mallp2);
    free(mallp3);
    free(mallp4);
    return 0;
}
