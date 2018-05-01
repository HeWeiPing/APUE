#include<pthread.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

int quitflag;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait = PTHREAD_COND_INITIALIZER;

void *thr_fn(void *arg)
{
	int err, signo;

	for( ; ; ) {
		err = sigwait(&mask, &signo);
		switch(signo) {
			case SIGINT:
				printf("\ninterrupt\n");
				break;
			case SIGQUIT:
				pthread_mutex_lock(&lock);
				quitflag = 1;
				pthread_mutex_unlock(&lock);
				pthread_cond_signal(&wait);
				return 0;
			default:
				printf("unexpected signal %d\n", signo);
				exit(1);
		}
	}
}

int main(void)
{
	int err;
	sigset_t oldmask;
	pthread_t tid;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	pthread_sigmask(SIG_BLOCK, &mask, &oldmask);

	pthread_create(&tid, NULL, thr_fn, 0);

	pthread_mutex_lock(&lock);
	while(quitflag == 0)
		pthread_cond_wait(&wait, &lock);
	pthread_mutex_unlock(&lock);

	quitflag = 0;

	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	exit(0);
}
