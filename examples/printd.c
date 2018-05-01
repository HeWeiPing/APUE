#include "apue.h"
#include "print.h"
#include "ipp.h"
#include<fcntl.h>
#include<dirent.h>
#include<ctype.h>
#include<pwd.h>
#include<pthread.h>
#include<strings.h>
#include<sys/select.h>
#include<sys/uio.h>

#define HTTP_INFO(x) ((x) >= 100 && (x) <= 199)
#define HTTP_SUCCESS(x) ((x) >= 200 && (x) <= 299)

struct job {
	struct job *next;
	struct job *prev;
	long jobid;
	struct printreq req;
};

struct worker_thread {
	struct worker_thread *next;
	struct worker_thread *prev;
	pthread_t tid;
	int sockfd;
};

int log_to_stderr = 0;

struct addrinfo *printer;
char *printer_name;
pthread_mutex_t configlock = PTHREAD_MUTEX_INITIALIZER;
int reread;

struct worker_thread *workers;
pthread_mutex_t workerlock = PTHREAD_MUTEX_INITIALIZER;
sigset_t mask;

struct job *jobhead, *jobtail;
int jobfd;
long nextjob;
pthread_mutex_t joblock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t jobwait = PTHREAD_COND_INITIALIZER;

void init_request(void);
void init_printer(void);
void update_jobno(void);
long get_newjobno(void);
void add_job(struct printreq *, long);
void replace_job(struct job *);
void remove_job(struct job *);
void build_qonstart(void);
void *client_thread(void *);
void *printer_thread(void *);
void *signal_thread(void *);
ssize_t readmore(int, char **, int, int *);
int printer_status(int, struct job *);
void add_worker(pthread_t, int);
void kill_workers(void);
void client_cleanup(void *);

int
main(int argc, char *argv[])
{
	pthread_t tid;
	struct addrinfo *ailist, *aip;
	int sockfd, err, i, n, maxfd;
	char *host;
	fd_set rendezvous, rset;
	struct sigaction sa;
	struct passwd *pwdp;

	if(argc != 1)
		err_quit("usage: printd");
	daemonize("printd");

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
	if(sigaction(SIGPIPE, &sa, NULL) < 0)
		log_sys("sigaction failed");
	sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGTERM);
	if((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		log_sys("pthread_sigmask failed");
	
	init_request();
	init_printer();

#ifdef _SC_HOST_NAME_MAX
	n = sysconf(_SC_HOST_NAME_MAX);
	if(n < 0)
#endif
		n = HOST_NAME_MAX;

	if((host = malloc(n)) == NULL)
		log_sys("malloc error");
	if(gethostname(host, n) < 0)
		log_sys("gethostname error");

    if((err = getaddrlist(host, "print", &ailist)) != 0) {
		log_quit("getaddrinfo error: %s", gai_strerror(err));
		exit(1);
	}

	FD_ZERO(&rendezvous);
	maxfd = -1;
	for(aip=ailist; aip!=NULL; aip=aip->ai_next) {
		if((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
			FD_SET(sockfd, &rendezvous);
			if(sockfd > maxfd)
				maxfd = sockfd;
		}
	}
	if(maxfd == -1)
		log_quit("service not enabled");

	pwdp = getpwnam("lp");
	if(pwdp == NULL)
		log_sys("can't find user lp");
	if(pwdp->pw_uid == 0)
		log_quit("user lp is privileged");
	if(setuid(pwdp->pw_uid) < 0)
		log_sys("can't change IDs to user lp");

	pthread_create(&tid, NULL, printer_thread, NULL);
	pthread_create(&tid, NULL, signal_thread, NULL);
	build_qonstart();

	log_msg("daemon initialized");

	for( ; ; ) {
		rset = rendezvous;
		if(select(maxfd+1, &rset, NULL, NULL, NULL) < 0)
			log_sys("select failed");
		for(i=0; i<=maxfd, i++) {
			if(FD_ISSET(i, &rset)) {
				sockfd = accept(i, NULL, NULL);
				if(sockfd < 0)
					log_ret("accept failed");
				pthread_create(&tid, NULL, client_signal, (void *)sockfd);
			}
		}
	}
	exit(0);
}

void
init_request(void)
{
	int n;
	char name[FILENMSZ];

	sprintf(name, "%s/%s", SPOOLDIR, JOBFILE);
	jobfd = open(name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	if(write_lock(jobfd, 0, SEEK_SET, 0) < 0)
		log_quit("daemon already running");

	if((n = read(jobfd, name, FILENMSZ)) < 0)
		log_sys("can't read job file");
	if(n == 0)
		nextjob = 1;
	else
		nextjob = atol(name);
}

void
init_printer(void)
{
	printer = get_printaddr();
	if(printer == NULL) {
		log_msg("no printer device registered");
		exit(1);
	}
	printer_name = printer->ai_canonname;
	if(printer_name == NULL)
		printer_name = "printer";
	log_msg("printer is %s", printer_name);
}

void
update_jobno(void)
{
	char buf[32];

	lseek(jobfd, 0, SEEK_SET);
	sprintf(buf, "%ld", nextjob);
	if(write(jobfd, buf, strlen(buf)) < 0)
		log_sys("can't update job file");
}

long
get_newjobno(void)
{
	long jobid;

	pthread_mutex_lock(&joblock);
	jobid = nextjob++;
	if(nextjob <= 0)
		nextjob = 1;
	pthread_mutex_unlock(&joblock);
	return jobid;
}

void 
add_job(struct printreq *reqp, long jobid)
{
	struct job *jp;

	if((jp = malloc(sizeof(struct job))) == NULL)
		log_sys("malloc failed");

	memcpy(&jp->req, reqp, sizeof(struct job));
	jp->jobid = jobid;

	jp->next = NULL;
	pthread_mutex_lock(&joblock);
	jp->prev = jobtail;
	if(jobtail == NULL)
		jobhead = jp;
	else
		jobtail->next = jp;
	jobtail = jp;
	pthread_mutex_unlock(&joblock);
	pthread_cond_signal(&jobwait);
}

void
replace_job(struct job *jp)
{
	pthread_mutex_lock(&joblock);
	jp->next = jobhead;
	jp->prev = NULL;
	if(jobhead == NULL)
		jobtail = jp;
	else
		jobhead->prev = jp;
	jobhead = jp;
	pthread_mutex_unlock(&joblock);
}

void
remove_job(struct job *target)
{
	if(target->next != NULL)
		target->next->prev = target->prev;
	else
		jobtail = target->prev;

	if(target->prev != NULL)
		target->prev->next = target->next;
	else
		jobhead = target->next;
}

void 
build_qonstart(void)
{
	int fd, err, nr;
	long jobid;
	DIR *dirp;
	struct dirent *entp;
	struct printreq req;
	char dname[FILENMSZ], fname[FILENMSZ];

	sprintf(dname, "%s/%s", SPOOLDIR, REQDIR);
	if((dirp = opendir(dname)) == NULL)
		return;
	while((entp = readdir(dirp)) != NULL) {
		if(strcmp(entp->d_name, ".") == 0 || strcmp(entp->d_name, ".."))
			continue;
		sprintf(fname, "%s/%s/%s", SPOOLDIR, REQDIR, entp->d_name);
		if((fd = open(fname, O_RDONLY)) < 0)
			continue;
		nr = read(fd, &req, sizeof(struct printreq));
		if(nr != sizeof(struct printreq)) {
			if(nr < 0)
				err = errno;
			else
				err = EIO;
			close(fd);
			log_msg("build_qonstart: can't read %s: %s", fname, strerror(err));
			unlink(fname);
			sprintf(fname, "%s/%s/%s", SPOOLDIR, DATADIR, entp->d_name);
			unlink(frame);
			continue;
		}
		jobid = atol(entp->d_name);
		log_msg("adding job %ld to queue", jobid);
		add_job(&req, jobid);
	}
	closedir(dirp);
}

void *
client_thread(void *arg)
{
	int n, fd, sockfd, nr, nw, first;
	long jobid;
	pthread_t tid;
	struct printreq req;
	struct printresp res;
	char name[FILENMSZ];
	char buf[IOBUFSZ];

	tid = pthread_self();
	pthread_cleanup_push(client_cleanup, (void *)tid);
	sockfd = (int)arg;
	add_worker(tid, sockfd);

	if((n = treadn(sockfd, &req, sizeof(struct printreq), 10)) != sizeof(struct printreq)) {
		res.jobid = 0;
		if(n < 0)
			res.retcode = htonl(errno);
		else
			res.retcode = htonl(EIO);
		strncpy(res.msg, strerror(res.retcode), MSGLEN_MAX);
		writen(sockfd, &res, sizeof(struct printresp));
		pthread_exit((void *)1);
	}
	req.size = ntohl(req.size);
	req.flags = ntohl(req.flags);

	jobid = get_newjobno();
	sprintf(name, "%s/%s/%ld", SPOOLDIR, DATADIR, jobid);
	if((fd = creat(name, FILEPERM)) < 0) {
		res.jobid = 0;
		if(n < 0)
			res.retcode = htonl(errno);
		else
			res.retcode = htonl(EIO);
		log_msg("client_thread: can't create %s : %s", name, strerror(res.retcode));
		strncpy(res.msg, strerror(res.retcode), MSGLEN_MAX);
		writen(sockfd, &res, sizeof(printresp));
		pthread_exit((void *)1);
	}

	first = 1;
	while((nr = tread(sockfd, buf, IOBUFSZ, 20)) > 0) {
		if(first) {
			first = 0;
			if(strncmp(buf, "%!PS", 4) == 0)
				req.flags |= PR_TEXT;
		}

		nw = write(fd, buf, nr);
		if(nw != nr) {
			/* error handle */
		}
	}
	close(fd);

	sprintf(name, "%s/%s/%ld", SPOOLDIR, REQDIR, jobid);
	fd = creat(name, FILEPERM);
	if(fd < 0) {
		/* error handle */
	}
	nw = write(fd, &req, sizeof(struct printreq));
	if(nw != sizeof(struct printreq)) {
		/* error handle */
	}
	close(fd);

	res.retcode = 0;
	res.jobid = htonl(jobid);
	sprintf(res.msg, "request ID %ld", jobid);
	writen(sockfd, &res, sizeof(struct printresp));

	log_msg("adding job %ld to queue", jobid);
	add_job(&req, jobid);
	pthread_cleanup_pop(1);

	return ((void *)0);
}

void
add_worker(pthread_t tid, int sockfd)
{
	struct worker_thread *wtp;
	if((wtp = malloc(sizeof(struct worker_thread))) == NULL) {
		log_ret("add_worker: can't alloc");
		pthread_exit((void *)1);
	}

	wtp->tid = tid;
	wtp->sockfd = sockfd;
	pthread_mutex_lock(&workerlock);
	wtp->prev = NULL;
	wtp->next = workers;
	if(workers == NULL)
		workers = wtp;
	else
		workers->prev = wtp;
	pthread_mutex_unlock(&workerlock);
}

void 
kill_workers(void)
{
	struct worker_thread *wtp;

	pthread_mutex_lock(&workerlock);
	for(wtp=workers; wtp!=NULL; wtp=wtp->next)
		pthread_cancel(wtp->tid);
	pthread_mutex_unlock(&workerlock);
}

void 
client_cleanup(void *arg)
{
	struct worker_thread *wtp;
	pthread_t tid;

	tid = (pthread_t)arg;
	pthread_mutex_lock(&workerlock);
	for(wtp=workers; wtp!=NULL; wtp=wtp->next) {
		if(wtp->tid == tid) {
			if(wtp->next != NULL)
				wtp->next->prev = wtp->prev;
			if(wtp->prev != NULL)
				wtp->prev->next = wtp->next;
			else
				workers = wtp->next;
			break;
		}
	}
	pthread_mutex_unlock(&workerlock);
	if(wtp != NULL) {
		close(wtp->sockfd);
		free(wtp);
	}
}

void *
signal_thread(void *arg)
{
	int err, signo;

	for( ; ; ) {
		err = sigwait(&mask, &signo);
		if(err != 0)
			log_quit("sigwait failed: %s", strerror(err));
		switch(signo) {
			case SIGHUP:
				pthread_mutex_lock(&configlock);
				reread = 1;
				pthread_mutex_unlock(&configlock);
				break;
			case SIGTERM:
				kill_workers();
				log_msg("terminate with signal %s", strsignal(signo));
				exit(0);
			default:
				kill_workers();
				log_quit("unexpected signal %d", signo);
		}
	}
}

char *
add_option(char *cp, int tag, char *optname, char *optval)
{
}

void *
printer_thread(void *arg)
{
	struct job *jp;
	int hlen, ilen, sockfd, fd, nr, nw;
	char *icp, *hcp;
	struct ipp_hdr *hp;
	struct stat sbuf;
	struct iovec iov[2];
	char name[FILENMSZ];
	char hbuf[HBUFSZ];
	char ibuf[IBUFSZ];
	char buf[IOBUFSZ];
	char str[64];

	for( ; ; ) {
		pthread_mutex_lock(&joblock);
		while(jobhead == NULL) {
			log_msg("printer_thread: waiting...");
			pthread_cond_wait(&jobwait, &joblock);
		}
		remove_job(jp=jobhead);
		log_msg("printer_thread: picked up job %ld", jp->jobid);
		pthread_mutex_unlock(&joblock);

		update_jobno();

		pthread_mutex_lock(&configlock);
		if(reread) {
			freeaddrinfo(printer);
			printer = NULL;
			printer_name = NULL;
			reread = 0;
			pthread_mutex_unlock(&configlock);
			init_printer();
		} else
			pthread_mutex_unlock(&configlock);

		sprintf(name, "%s/%s/%ld", SPOOLDIR, DATADIR, jp->jobid);
		if((fd = open(name, O_RDONLY)) < 0) {
			log_msg("job %ld canceled - can't open %s: %s", job->jobid, name, strerror(errno));
			free(jp);
			continue;
		}
		if(fstat(fd, &sbuf) < 0) {
			/* error handle */
		}
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			/* error handle */
		}
		if(connect_retry(sockfd, printer->ai_addr, printer->ai_addrlen) < 0) {
			/* errno handle */
		}

		/* set up the IPP header */

		/* set up the HTTP header */

		iov[0].iov_base = hbuf;
		iov[0].iov_len = hlen;
		iov[1].iov_base = ibuf;
		iov[1].iov_len = ilen;

		if((nw = writev(sockfd, iov, 2)) != hlen+ilen) {
			/* error handle */
		}
		while((nr = read(fd, buf, IOBUFSZ)) > 0) {
			if((nw = write(sockfd, buf, nr)) != nr) {
				/* error handle */
			}
		}

		/* read response from the printer */
	}
}
       
/* ...... */
