//使用程序清单 8-12.c 的system函数版本

#include<signal.h>
#include<stdlib.h>
#include<stdio.h>

extern void err_sys(const char *, ...);
extern int system(const char *);

static void sig_int(int signo)
{
	printf("caught SIGINT\n");
}

static void sig_chld(int signo)
{
	printf("caught SIGCHLD\n");
}

int main(void)
{
	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	if(signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal(SIGCHLD) error");

	if(system("/bin/ed") < 0)
		err_sys("system() error");
	return 0;
}
