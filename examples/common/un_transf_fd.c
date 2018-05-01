#include "apue.h"
#include<sys/socket.h>

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

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr_s = NULL;

int send_fd(int fd, int fd_to_send)
{
	struct iovec iov[1];
	struct msghdr msg;
	char buf[2];

	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	if(fd_to_send < 0) {
		buf[1] = -fd_to_send;
		if(buf[1] == 0)
			buf[1] = 1;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
	} else {
		if(cmptr_s == NULL)
			cmptr_s = malloc(CONTROLLEN);
		if(cmptr_s == NULL)
			return -1;

		cmptr_s->cmsg_level = SOL_SOCKET;
		cmptr_s->cmsg_type = SCM_RIGHTS;
		cmptr_s->cmsg_len = CONTROLLEN;
		msg.msg_control = cmptr_s;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr_s) = fd_to_send;
		buf[1] = 0;
	}
	buf[0] = 0;
	if(sendmsg(fd, &msg, 0) != 2)
		return -1;
	return 0;
}

#define MAXL 1024
static struct cmsghdr *cmptr_r = NULL;

int recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t))
{
	int newfd, nr, status;
	char *ptr;
	char buf[MAXL];
	struct iovec iov[1];
	struct msghdr msg;

	status = -1;
	for( ; ; ) {
		iov[0].iov_base = buf;
		iov[0].iov_len = MAXL;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		if(cmptr_r==NULL && (cmptr_r=malloc(CONTROLLEN))==NULL)
			return -1;
		msg.msg_control = cmptr_r;
		msg.msg_controllen = CONTROLLEN;

		if((nr = recvmsg(fd, &msg, 0)) < 0)
			err_sys("recvmsg error");
		else if(nr == 0) {
			err_ret("connection closed by server");
			return -1;
		}

		for(ptr=buf; ptr<&buf[nr]; ) {
			if(*ptr++ == 0) {
				if(ptr != &buf[nr-1])
					err_dump("message format error");
				status = *ptr & 0xFF;
				if(status == 0) {
					if(msg.msg_controllen != CONTROLLEN)
						err_dump("status = 0 but no fd");
					newfd = *(int *)CMSG_DATA(cmptr_r);
				} else
					newfd = -status;
				nr -= 2;
			}
		}
		if(nr > 0)
			if((*userfunc)(STDERR_FILENO, buf, nr) != nr)
				return -1;
		if(status >= 0)
			return newfd;
	}
}


