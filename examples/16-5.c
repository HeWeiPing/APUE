#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<errno.h>
#include<syslog.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define BUFLEN 128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, struct sockaddr *, socklen_t, int);
extern void err_quit(const char *, ...);

void serve(int sockfd)
{
	int clfd;
	FILE *fp;
	char buf[BUFLEN];

	for( ; ; ) {
		clfd = accept(sockfd, NULL, NULL);
		if(clfd < 0) {
			syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
			exit(1);
		}
		if((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			sprintf(buf, "error: %s\n", strerror(errno));
			send(clfd, buf, strlen(buf), 0);
		} else {
			while(fgets(buf, BUFLEN, fp) != NULL)
				send(clfd, buf, strlen(buf), 0);
			pclose(fp);
		}
		close(clfd);
	}
}

int main(int argc, char *argv[])
{
	struct addrinfo *ailist, *aip;
	struct addrinfo hint;
	int sockfd, err, n;
	char *host;

	if(argc != 1)
		err_quit("usage: ruptimed");
#ifdef _SC_HOST_NAME_MAX
	n = sysconf(_SC_HOST_NAME_MAX);
	if(n < 0)
#endif
	n = HOST_NAME_MAX;
	host = malloc(n);
	gethostname(host, n);

	daemonize("ruptimed");

	hint.ai_flags = 0;
	hint.ai_family = 0;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	if((err = getaddrinfo(host, "ruptime_sv", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}

	for(aip=ailist; aip!=NULL; aip=aip->ai_next) {
		if((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
			serve(sockfd);
			exit(0);
		}
	}
	exit(1);
}
