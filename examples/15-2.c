#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

#define DEF_PAGER "/bin/more"
#define MAXL 4096

extern void err_quit(const char *, ...);
extern void err_sys(const char *, ...);

/*
 * 稍作改动，测试捕捉SIGCHLD信号后，waitpid函数有否有异常
 * 结果发现，信号处理程序会执行，而且waitpid函数也成功返回
 */
static void sig_chld(int signo)
{
	if(signo == SIGCHLD)
		printf("receved SIGCHLD...in signal handler\n");
}

int main(int argc, char *argv[])
{
	int n, status;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[MAXL];
	FILE *fp;

	if(argc != 2)
		err_quit("usage: a.out <pathname>");

	signal(SIGCHLD, sig_chld);

	if((fp = fopen(argv[1], "r")) == NULL)
		err_sys("fopen: %s error", argv[1]);
	if(pipe(fd) < 0)
		err_sys("pipe error");

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0) { //parent
		close(fd[0]);

		while(fgets(line, MAXL, fp) != NULL) {
			n = strlen(line);
			if(write(fd[1], line, n) != n)
				err_sys("write error to pipe");
		}
		if(ferror(fp))
			err_sys("fgets error");

		close(fd[1]);

		if(waitpid(pid, &status, 0) < 0)
			err_sys("waitpid error: %s", strerror(errno));
		else
			printf("waitpid successful: %d\n", status);

		exit(0);
	} else { //child
        close(fd[1]);

		if(fd[0] != STDIN_FILENO) {
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
		}

		if((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		if((argv0 = strrchr(pager, '/')) != NULL)
			argv0++;
		else
			argv0 = pager;

		if(execl(pager, argv0, (char *)0) < 0)
			err_sys("execl error for %s", pager);
	}
	exit(0);
}
