#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

/*
 * 本程序并非APUE中的程序清单4-5
 * 目的是测试link函数的工作方式
 *
 * 不添加磁盘容量，仅仅修改i-node节点的链接计数
 * 运行程序前后，用du命令查看当前目录的大小
 */
int main(void)
{
	struct stat statbuff1, statbuff2;

	if(stat("err_handle.c", &statbuff1) < 0)
		err_sys("stat error for err_handle.c");

	printf("err_handle.c: \n");
	printf("i-node number: %d\n", statbuff1.st_ino);
	printf("number of links: %d\n", statbuff1.st_nlink);
	printf("size in bytes: %d\n", statbuff1.st_size);

	link("err_handle.c", "err_link.c");

	if(stat("err_link.c", &statbuff2) < 0)
		err_sys("stat error for err_link.c");

	printf("\nerr_link.c: \n");
	printf("i-node number: %d\n", statbuff2.st_ino);
	printf("number of links: %d\n", statbuff2.st_nlink);
	printf("size in bytes: %d\n", statbuff2.st_size);

	exit(0);
}
