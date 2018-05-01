#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<stdlib.h>

extern void err_sys(const char *, ...);

char *env_init[] = {
	"USER=zhaoli",
	"PATH=/tmp",
	NULL
};

int main(void)
{
	pid_t pid;

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid == 0) {   //child process
		if(execle("/home/simplyzhao/code/unix-c/apue/8-9", "echo", "argv1", "argv2", (char *)0, env_init) < 0)
			err_sys("execle error");
	}

	if(waitpid(pid, NULL, 0) < 0)
		err_sys("fork error");

	exit(0);
}
