#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAXL 1024

static void sig_pipe(int);

int main(void)
{
	int n, fd1[2], fd2[2];
	pid_t pid;
	char line[MAXL];

	if(signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");

	if(pipe(fd1) < 0 || pipe(fd2) < 0)
		err_sys("pipe error");

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0){
		close(fd1[0]);
		close(fd2[1]);
		while(fgets(line, MAXL, stdin) != NULL) {
			n = strlen(line);
			write(fd1[1], line, n);
			if((n = read(fd2[0], line, MAXL)) < 0)
				err_sys("read error from pipe");
			if(n == 0) {
				err_msg("child closed pipe");
				break;
			}
			line[n] = 0;
			fputs(line, stdout);
		}
		exit(0);
	} else {
		close(fd1[1]);
		close(fd2[0]);
		if(fd1[0] != STDIN_FILENO) {
			dup2(fd1[0], STDIN_FILENO);
			close(fd1[0]);
		}
		if(fd2[1] != STDOUT_FILENO) {
			dup2(fd2[1], STDOUT_FILENO);
			close(fd2[1]);
		}
		execl("./add2", "add2", (char *)0);
	}
	exit(0);
}

static void sig_pipe(int signo)
{
	printf("SIGPIPE caught\n");
	exit(1);
}

