#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAXL 1024
static void sig_pipe(int);

int main(void)
{
	int n;
	int fd[2];
	pid_t pid;
	char line[MAXL];

	signal(SIGPIPE, sig_pipe);

	if(s_pipe(fd) < 0)
		err_sys("pipe error");

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0) {
		close(fd[1]);
		while(fgets(line, MAXL, stdin) != NULL) {
			n = strlen(line);
			if(write(fd[0], line, n) != n)
				err_sys("write error to pipe");

			if((n = read(fd[0], line, MAXL)) < 0)
				err_sys("read error from pipe");

			if(n == 0) {
				err_msg("child closed pipe");
				break;
			}
			line[n] = 0;
			fputs(line, stdout);
		}
		if(ferror(stdin))
			err_sys("fgets error on stdin");
	} else {
        close(fd[0]);
		if(fd[1] != STDIN_FILENO) {
			dup2(fd[1], STDIN_FILENO);
		}
		if(fd[1] != STDOUT_FILENO) {
			dup2(fd[1], STDOUT_FILENO);
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

