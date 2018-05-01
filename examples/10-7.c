#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

#define MAXLINE 4096

static void sig_alrm(int);

int main(void)
{
	int n;
	char line[MAXLINE];

	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	alarm(1);
	//这里存在竞争条件
	//依赖 中断的系统调用， 如果系统自动重启系统调用，则无效
	if((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	alarm(0);

	write(STDOUT_FILENO, line, n);
	return 0;
}

static void sig_alrm(int signo)
{
	//do nothing
}
