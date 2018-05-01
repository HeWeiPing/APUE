#include<unistd.h>
#include<sys/utsname.h>
#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	struct utsname utsbuf;

	if(uname(&utsbuf) < 0)
		err_sys("uname error...");

	printf("--------uname---------\n");
	printf("sysname: %s\n", utsbuf.sysname);
	printf("nodename: %s\n", utsbuf.nodename);
	printf("release: %s\n", utsbuf.release);
	printf("version: %s\n", utsbuf.version);
	printf("machine: %s\n", utsbuf.machine);

	printf("\n-------------gethostname---------------\n");

	char hostnam[65];
	gethostname(hostnam, 65);
	printf("hostname: %s\n", hostnam);

	exit(0);
}
