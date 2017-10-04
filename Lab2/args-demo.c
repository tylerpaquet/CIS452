#include <stdio.h>

/* Run this program and supply command line arguments
   ./a.out hello world
   ./a.out "hello world" 123 456
 */

extern char** environ;

int main (int argc, char* argv[], char*env[]) {
    int k;
    /* For all command line arguments */
    for (k = 0; k < argc; k++)
        printf ("Arg-%02d is %s\n", k, argv[k]);

#if 0
    for (k = 0; env[k]; k++)
        printf ("Env-%02d is %s\n", k, env[k]);
    for (k = 0; environ[k]; k++)
        printf ("Environ-%02d is %s\n", k, environ[k]);
#endif
    return 0;
}
