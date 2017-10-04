//
//1b
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define SIZE 30

int main (int argc, char *argv[]) {
    struct flock fileLock;
    int fd;
    char buf[SIZE];

    if (argc < 2) {
        printf ("usage: filename\n");
        exit (1);
    }
    if ((fd = open (argv[1], O_RDWR)) < 0) {
        perror ("there is");
        exit (1);
    }

    fileLock.l_type = F_WRLCK;
    fileLock.l_whence = SEEK_SET;
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    while (fcntl (fd, F_SETLK, &fileLock) < 0)
    {
        wait();
    }
    
    

    read (fd, buf, SIZE - 2);
    printf("%s\n", buf);
    
    fileLock.l_type = F_UNLCK;
    fcntl (fd, F_SETLK, &fileLock);

    close (fd);

    return 0;
}
