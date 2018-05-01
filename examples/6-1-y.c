#include<shadow.h>
#include<stdio.h>
#include<stdlib.h>

/*
 * 编译后需要执行以下三条命令才能运行a.out:
 * su
 * chown root a.out
 * chmod u+s a.out
 *
 * 设置用户ID位
 * 将进程的有效用户ID设为文件(a.out)所有者的用户ID(这里是root)
 */
int main(int argc, char *argv[])
{
	if(argc != 2)
		err_quit("usage: a.out <username>");

	struct spwd *spwdbuf;
	if((spwdbuf = getspnam(argv[1])) == NULL)
		err_sys("getspnam %s error", argv[1]);

	printf("User: %s\n", spwdbuf->sp_namp);
	printf("Password: %s\n", spwdbuf->sp_pwdp);

	exit(0);
}
