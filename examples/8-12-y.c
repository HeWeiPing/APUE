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
		sleep(2);
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
	} else {   //parent process
		//WNOHANG参数指示父进程不会阻塞	
		if(waitpid(pid, &status, WNOHANG) < 0)
			status = -1;
	}

	return status;
}

int main(void)
{
	system("date");

	return 0;
}

