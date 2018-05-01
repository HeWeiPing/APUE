#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	off_t currpos;

	if((currpos = lseek(STDIN_FILENO, 0, SEEK_CUR)) == -1)
		printf("Can't seek\n");
	else
		printf("Seek OK\n");

	exit(0);
}
