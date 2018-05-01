#include<signal.h>
#include<stdio.h>

// sleep2函数在文件10-5.c中定义
extern unsigned int sleep2(unsigned int);
extern void err_sys(const char *, ...);
static void sig_int(int);

int main(void)
{
	unsigned int unslept;

	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	unslept = sleep2(3);
	printf("sleep2 returned: %u\n", unslept);
	
	return 0;
}

static void sig_int(int signo)
{
	int i, j;
	volatile int k;

	printf("\nsig_int starting...\n");
	for(i=0; i<300000; i++)
		for(j=0; j<4000; j++)
			k+= i*j;
	printf("sig_int finished\n");
}

