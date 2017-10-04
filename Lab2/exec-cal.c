#include <unistd.h>
#include <stdio.h>

int main() {
    printf (">>>>>>>\n");
    execl ("/usr/bin/calxxx", "Venus", NULL);
    printf ("<<<<<<<\n");
    return 0;
}

#if 0
int main() {
    printf (">>>>>>>\n");
    /* Show calendar three months around Feb 2020 */
    execl ("/usr/bin/cal", "This string may not be important", "-3", "2", "2020", NULL);
    printf ("<<<<<<<\n");
    return 0;
}

int main() {
    printf (">>>>>>>\n");
    char *args[] = {"/usr/bin/cal", NULL};
    execv (args[0], args);
    printf ("<<<<<<<\n");
    return 0;
}

int main() {
    printf (">>>>>>>\n");
    char *args[] = {"/usr/bin/cal", "-3", "2", "2020", NULL};
    execv (args[0], args);
    printf ("<<<<<<<\n");
    return 0;
}
#endif
