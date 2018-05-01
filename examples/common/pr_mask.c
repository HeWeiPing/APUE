#include<signal.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>

extern void err_sys(const char *, ...);

void pr_mask(const char *str)
{
	sigset_t sigset;
	int errno_save;

	errno_save = errno;
	if(sigprocmask(0, NULL, &sigset) < 0)
		err_sys("sigprocmask error");

	printf("%s", str);
	if(sigismember(&sigset, SIGABRT)) printf("SIGABRT ");
    if(sigismember(&sigset, SIGALRM)) printf("SIGALRM ");
#ifdef SIGBUS
	if(sigismember(&sigset, SIGBUS)) printf("SIGBUS ");
#endif
	if(sigismember(&sigset, SIGCHLD)) printf("SIGCHLD ");
	if(sigismember(&sigset, SIGCONT)) printf("SIGCONT ");
#ifdef SIGEMT
	if(sigismember(&sigset, SIGEMT)) printf("SIGEMT ");
#endif
	if(sigismember(&sigset, SIGFPE)) printf("SIGFPE ");
	if(sigismember(&sigset, SIGHUP)) printf("SIGHUP ");
    if(sigismember(&sigset, SIGILL)) printf("SIGILL ");
	if(sigismember(&sigset, SIGINT)) printf("SIGINT ");
#ifdef SIGIO
	if(sigismember(&sigset, SIGIO)) printf("SIGIO ");
#endif
	if(sigismember(&sigset, SIGKILL)) printf("SIGKILL ");
	if(sigismember(&sigset, SIGPIPE)) printf("SIGPIPE ");
#ifdef SIGPOLL
	if(sigismember(&sigset, SIGPOLL)) printf("SIGPOLL ");
#endif
	if(sigismember(&sigset, SIGQUIT)) printf("SIGQUIT ");
	if(sigismember(&sigset, SIGSEGV)) printf("SIGSEGV ");
	if(sigismember(&sigset, SIGSTOP)) printf("SIGSTOP ");
#ifdef SIGSYS 
	if(sigismember(&sigset, SIGSYS)) printf("SIGSYS ");
#endif
	if(sigismember(&sigset, SIGTERM)) printf("SIGTERM ");
	if(sigismember(&sigset, SIGTSTP)) printf("SIGTSTP ");
	if(sigismember(&sigset, SIGTTIN)) printf("SIGTTIN ");
	if(sigismember(&sigset, SIGTTOU)) printf("SIGTTOU ");
	if(sigismember(&sigset, SIGUSR1)) printf("SIGUSR1 ");
	if(sigismember(&sigset, SIGUSR2)) printf("SIGUSR2 ");
#ifdef SIGWINCH
	if(sigismember(&sigset, SIGWINCH)) printf("SIGWINCH ");
#endif
    
	printf("\n");
	fflush(stdout);
}



