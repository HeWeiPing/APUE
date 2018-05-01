#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

/*
 * 对程序4-5稍作修改:
 * link -> symlink
 * stat -> lstat
 *
 * 目的是测试symlink函数的工作方式
 * 
 * 增加新的i-node来表示符号链接
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

	symlink("err_handle.c", "err_link.c");

	if(lstat("err_link.c", &statbuff2) < 0)
		err_sys("stat error for err_link.c");

	printf("\nerr_link.c: \n");
	printf("i-node number: %d\n", statbuff2.st_ino);
	printf("number of links: %d\n", statbuff2.st_nlink);
	printf("size in bytes: %d\n", statbuff2.st_size);

	exit(0);
}
