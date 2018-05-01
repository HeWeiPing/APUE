#include<unistd.h>
#include<pwd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern void err_sys(const char *, ...);

static void my_alarm(int signo)
{
	struct passwd *rootptr;

	printf("in signal handler\n");
	if((rootptr = getpwnam("root")) == NULL)
		err_sys("getpwnam(root) error");
	alarm(1);
}

int main(void)
{
	struct passwd *ptr;
	signal(SIGALRM, my_alarm);
	alarm(1);
	for( ; ; ) {
		if((ptr = getpwnam("simplyzhao")) == NULL)
			err_sys("getpwnam(simplyzhao) error");
		if(strcmp(ptr->pw_name, "simplyzhao") != 0)
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
	}
}
