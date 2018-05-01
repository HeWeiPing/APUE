#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

extern void err_quit(const char *, ...);

void *thr_fn1(void *arg)
{
	printf("thread 1 returning...\n");
	return ((void *)1);
}

void *thr_fn2(void *arg)
{
	printf("thread 2 returning...\n");
	return ((void *)2);
}

int main(void)
{
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if(err != 0)
		err_quit("Can't create thread 1: %s\n", strerror(err));
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if(err != 0)
		err_quit("Can't create thread 2: %s\n", strerror(err));

	if((err = pthread_join(tid1, &tret)))
		err_quit("Can't join with thread 1: %s\n", strerror(err));
	printf("thread 1 exit code %d\n", (int)tret);

	sleep(2);
	if((err = pthread_join(tid2, &tret)))
		err_quit("Can't join with thread 2: %s\n", strerror(err));
	printf("thread 2 exit code %d\n", (int)tret);

	exit(0);
}
