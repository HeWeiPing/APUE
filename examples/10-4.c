#include<unistd.h>
#include<signal.h>

static void sig_alrm(int signo)
{
	/* do nothing */
}

unsigned int sleep1(unsigned int nsecs)
{
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		return nsecs;

	alarm(nsecs);
	//这里存在竞争条件
	pause();
	return (alarm(0));
}
