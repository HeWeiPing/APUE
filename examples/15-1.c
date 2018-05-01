#include<unistd.h>
#include<stdlib.h>

#define MAXL 1024

int main(void)
{
	int fd[2];
	int n;
	pid_t pid;
	char line[MAXL];

	if(pipe(fd) < 0)
		err_sys("pipe error");
	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0) {   //parent
		close(fd[0]);
		write(fd[1], "hello world\n", 12);
	} else {   //child
		close(fd[1]);
		n = read(fd[0], line, MAXL);
		write(STDOUT_FILENO, line, n);
	}
	exit(0);
}
