#include<unistd.h>
#include<sys/wait.h>
#include<errno.h>

int system(const char *cmdstring)
{
	pid_t pid;
	int status;

	if(cmdstring == NULL)
		return 1;

	if((pid = fork()) < 0)
		status = -1;
	else if(pid == 0) {   //child process
		//测试父进程是否会阻塞
		sleep(2);
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
	} else {   //parent process
		if(waitpid(pid, &status, 0) < 0)
			status = -1;
	}

	return status;
}

int main(void)
{
	system("date");

	return 0;
}

