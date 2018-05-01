#include "apue.h"
#include<fcntl.h>
#include<stropts.h>

/*
 * 创建无关进程间的唯一连接
 * STREAMS 流机制 作为底层通信机制
 */

#define FIFO_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int serv_listen(const char *name)
{
	int tempfd;
	int fd[2];

	unlink(name);
	if((tempfd = creat(name, FIFO_MODE)) < 0)
		return -1;
	if(close(tempfd) < 0)
		return -2;
	if(pipe(fd) < 0)
		return -3;

	//Push connld & fattach on fd[1]
	if(ioctl(fd[1], I_PUSH, "connld") < 0) {
		close(fd[0]);
		close(fd[1]);
		return -4;
	}
	if(fattach(fd[1], name) < 0) {
		close(fd[0]);
		close(fd[1]);
		return -5;
	}
	close(fd[1]);
	return fd[0];
}

int serv_accept(int listenfd, uid_t *uidptr)
{
	struct strrecvfd recvfd;

	if(ioctl(listenfd, I_RECVFD, &recvfd) < 0)
		return -1;
	if(uidptr != NULL)
		*uidptr = recvfd.uid;
	return recvfd.fd;
}

int cli_conn(const char *name)
{
	int fd;

	if((fd = open(name, O_RDWR)) < 0)
		return -1;
	if(isastream(fd) == 0) {
		close(fd);
		return -2;
	}
	return fd;
}
