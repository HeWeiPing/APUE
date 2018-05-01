#include<stdio.h>
#include<stdlib.h>

#define MAXL 1024

int main(void)
{
	FILE *fpin;
	char line[MAXL];

	fpin = popen("./15-6", "r");

	for( ; ; ) {
		fputs("prompt> ", stdout);
		fflush(stdout);
		if(fgets(line, MAXL, fpin) == NULL)
			break;
		if(fputs(line, stdout) == EOF)
			err_sys("fputs error to pipe");
	}

	pclose(fpin);
	putchar('\n');
	exit(0);
}
