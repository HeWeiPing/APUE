#include "apue.h"
#include<stropts.h>

/*
 * 文件描述符用两个ioctl命令经由STREAMS管道交换
 * I_SENDFD
 * I_RECVFD
 */
int send_err(int fd, int errcode, const char *msg)
{
	int n;
	if((n = strlen(msg)) > 0)
		if(writen(fd, msg, n) != n)
			return -1;

	if(errcode >= 0)
		errcode = -1;

	if(send_fd(fd, errcode) < 0)
		return -1;

	return 0;
}

int send_fd(int fd, int fd_to_send)
{
	char buf[2];
	buf[0] = 0;
	if(fd_to_send < 0) {
		buf[1] = -fd_to_send;
		if(buf[1] == 0)
			buf[1] = 1;
	} else
		buf[1] = 0;

	if(write(fd, buf, 2) != 2)
		return -1;

	if(fd_to_send >= 0)
		if(ioctl(fd, I_SENDFD, fd_to_send) < 0)
			return -1;
	return 0;
}

#define MAXL 1024
int recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t))
{
	int newfd, nread, flag, status;
	char *ptr;
	char buf[MAXL];
	struct strbuf dat;
	struct strrecvfd recvfd;

	status = -1;
	for( ; ; ) {
		dat.buf = buf;
		dat.maxlen = MAXL;
		flag = 0;
		if(getmsg(fd, NULL, &dat, &flag) < 0)
			err_sys("getmsg error");
		nread = dat.len;
		if(nread == 0) {
			err_ret("connection closed by server");
			return -1;
		}

		for(ptr=buf; ptr<&buf[nread]; ) {
			if(*ptr++ == 0) {
				if(ptr != &buf[nread-1])
					err_dump("message format error");
				status = *ptr & 0xFF;
				if(status == 0) {
					if(ioctl(fd, I_RECVFD, &recvfd) < 0)
						return -1;
					newfd = recvfd.fd;
				} else
					newfd = -status;
				nread -= 2;
			}
		}
		if(nread > 0)
			if((*userfunc)(STDERR_FILENO, buf, nread) != nread)
				return -1;

		if(status >= 0)
			return newfd;
	}
}
