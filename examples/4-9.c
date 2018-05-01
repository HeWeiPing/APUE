#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

char *path_alloc(int *);

int main(void)
{
	char *ptr;
	int size;

	if(chdir("/home/simplyzhao/code") < 0)
		err_sys("chdir failed");

	ptr = path_alloc(&size);
	if(getcwd(ptr, size) == NULL)
		err_sys("getcwd failed");

	printf("cwd = %s\n", ptr);
	exit(0);
}
