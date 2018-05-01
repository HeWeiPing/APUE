#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int glob = 6; 

int main(void)
{
	int var;
	pid_t pid;

	var = 88;
	printf("before vfork\n");
	if((pid = vfork()) < 0)
		err_sys("vfork error");
	else if(pid == 0) {
		glob++;
		var++;
		//这里将_exit函数修改为exit函数查看运行结果是否相同
		exit(0);
	}

	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	exit(0);
}
