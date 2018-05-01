#include "apue.h"
#include<sys/socket.h>

//创建一对相互连接的UNIX域套接字
int s_pipe(int fd[2])
{
	return (socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}
