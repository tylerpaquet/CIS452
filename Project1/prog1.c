/*
 *Author: Tyler Paquet
 *CIS452
 *Due Date: January 19, 2017
 *Program 1
 */
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <pwd.h>
#include <stdlib.h>
int main()
{
	printf("User is: %s\n", getlogin());
	printf("UID is: %d\n", getuid());
	printf("GID is: %d\n", getgid());
	char hostname[20];
	gethostname(hostname, sizeof(hostname));
	printf("host is: %s\n", hostname);
	struct passwd pwd = *getpwent();
	printf("entry 0: %s\n", pwd.pw_name);
	struct passwd pwd2 = *getpwent();
	printf("entry 1: %s\n", pwd2.pw_name);
	struct passwd pwd3 = *getpwent();
	printf("entry 2: %s\n", pwd3.pw_name);
	struct passwd pwd4 = *getpwent();
	printf("entry 3: %s\n", pwd4.pw_name);
	struct passwd pwd5 = *getpwent();
	printf("entry 4: %s\n\n", pwd5.pw_name);
	printf("Enviroment variable desired? ");
	char var[30];
	scanf("%s", var);
	printf("\nvalue is: %s\n", getenv(var));
}
