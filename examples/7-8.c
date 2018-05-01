#include<sys/resource.h>
#include<stdio.h>
#include<stdlib.h>

#define doit(name) pr_limits(#name, name)

static void pr_limits(char *name, int resource)
{
	struct rlimit limit;

	if(getrlimit(resource, &limit) < 0)
		err_sys("getrlimit error for %s", name);
	printf("%-14s  ", name);
	if(limit.rlim_cur == RLIM_INFINITY)
		printf("(infinite)  ");
	else
		printf("%10ld", limit.rlim_cur);
	if(limit.rlim_max == RLIM_INFINITY)
		printf("(infinite)  ");
	else
		printf("%10ld", limit.rlim_max);

	putchar((int)'\n');
}

int main()
{
#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif
	doit(RLIMIT_CORE);
	doit(RLIMIT_CPU);
	doit(RLIMIT_DATA);
	doit(RLIMIT_FSIZE);
#ifdef RLIMIT_LOCKS
	doit(RLIMIT_LOCKS);
#endif

	exit(0);
}
