#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

typedef void (* sig_handler)(int);

extern void err_sys(const char *, ...);
extern void err_dump(const char *, ...);

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
		printf("received SIGUSR1\n");
	else if(signo == SIGUSR2)
		printf("received SIGUSR2\n");
	else
		err_dump("received signal %d\n", signo);
}

int main(void)
{
	sig_handler shdl = sig_usr;

	if(signal(SIGUSR1, shdl) == SIG_ERR)
		err_sys("can't catch SIGUSR1");
	if(signal(SIGUSR2, shdl) == SIG_ERR)
		err_sys("can't catch SIGUSR2");
	for( ; ; )
		pause();
}
