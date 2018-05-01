#ifndef _PRINT_H
#define _PRINT_H

#include<sys/socket.h>
#include<arpa/inet.h>
#if defined(BSD) || defined(MACOS)
#include<netinet/in.h>
#endif
#include<netdb.h>
#include<errno.h>

#define CONFIG_FILE "/etc/printer.conf"
#define SPOOLDIR "/var/spool/printer"
#define JOBFILE "jobno"
#define DATADIR "data"
#define REQDIR "reqs"

#define FILENMSZ 64
#define FILEPERM (S_IRUSR | S_IWUSR)
#define USERNM_MAX 64
#define JOBNM_MAX 256
#define MSGLEN_MAX 512

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

#define IPP_PORT 631
#define QLEN 10
#define IBUFSZ 512
#define HBUFSZ 512
#define IOBUFSZ 8192

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

extern int getaddrlist(const char *, const char *, struct addrinfo **);
extern char *get_printserver(void);
extern struct addrinfo *get_printaddr(void);
extern ssize_t tread(int, void *, size_t, unsigned int);
extern ssize_t treadn(int, void *, size_t, unsigned int);
extern int connect_retry(int, const struct sockaddr *, socklen_t);
extern int initserver(int, struct sockaddr *, socklen_t, int);

struct printreq {
	long size;
	long flags;
	char usernm[USERNM_MAX];
	char jobnm[JOBNM_MAX];
}

#define PR_TEXT 0x01

struct printresp {
	long retcode;
	long jobid;
	char msg[MSGLEN_MAX];
}

#endif /* _PRINT_H
