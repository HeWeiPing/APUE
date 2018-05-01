#include<unistd.h>
#include<errno.h>
#include<netdb.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAXADDRLEN 256
#define BUFLEN 128

// 16-2.c
extern int connect_retry(int, const struct sockaddr *, socklen_t); 
extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);

void print_uptime(int sockfd)
{
	int n;
	char buf[BUFLEN];
	while((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
		write(STDOUT_FILENO, buf, n);
	if(n < 0)
		err_sys("recv error");
}

int main(int argc, char *argv[])
{
	if(argc != 2)
		err_quit("usage: ruptime hostname");

	struct addrinfo *ailist, *aip;
	struct addrinfo hint;
	int sockfd, err;

	hint.ai_flags = 0;
	hint.ai_family = 0;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	if((err = getaddrinfo(argv[1], "ruptime_sv", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	for(aip=ailist; aip!=NULL; aip=aip->ai_next) {
		if((sockfd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0)
			err = errno;
		if(connect_retry(sockfd, aip->ai_addr, aip->ai_addrlen) < 0)
			err = errno;
		else {
			print_uptime(sockfd);
			exit(0);
		}
	}
	fprintf(stderr, "Can't connect to %s: %s\n", argv[1], strerror(err));
	exit(1);
}
