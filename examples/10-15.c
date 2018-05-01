#include<signal.h>
#include<stddef.h>

extern void err_sys(const char *, ...);
static void sig_int(int);
static void sig_quit(int);

int main(void)
{
	sigset_t newmask, oldmask, waitmask;

	pr_mask("program start: ");

	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	if(signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("signal(SIGQUIT) error");

	sigemptyset(&waitmask);
	sigaddset(&waitmask, SIGUSR1);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);

	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	/*
	 * Critical Region
	 */
	pr_mask("in critical region: ");

	if(sigsuspend(&waitmask) != -1)
		err_sys("sigsuspend error");
	pr_mask("after return from sigsuspend: ");

	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	pr_mask("program exit: ");
	
	return 0;
}

static void sig_int(int signo)
{
	pr_mask("in sig_int: ");
}

static void sig_quit(int signo)
{
	pr_mask("in sig_quit: ");
}
