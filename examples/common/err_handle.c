#include "apue.h"
#include<errno.h>
#include<stdarg.h>    /* ISO C variable arguments */

static void err_doit(int, int, const char *, va_list);

/*
 * Nonfatal error related to a system call
 * Print a message and return 
 */
void err_ret(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error related to a system call
 * Print a message and terminate
 */
void err_sys(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Fatal error unrelated to a system call 
 * Error code passed as explict paramater
 * Print a message and terminate
 */
void err_exit(int error, const char *fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Fatal error related to a system call
 * Print a message, dump core, and terminate
 */
void err_dump(const char*fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	/*
	 * the process has received a fatal signal, which has caused it to terminate and dump core. 
	 * Probably, the program called the abort() function, which causes it to send SIGABORT to itself.
	 * A core dump is a file containing the status of the program's memory space and the programs context status at the time it crashed.
	 * It's very useful for debugging. For example, if you find the core dump (usually named just "core"), you can run "gdb program -c core" 
	 * (where program is the path to the actual executable and core is the path to the core dump file),
	 * and you will be able to view what happened at the program's last moment (providing that you know how to use gdb, that is).
	 */
	abort();   /* dump core and terminate */
	exit(1);   /* shouldn't get here */
}

/*
 * Nonfatal error unrelated to a system call
 * Print a message and return 
 */
void err_msg(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error unrelated to a system call
 * Print a message and terminate
 */
void err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];
	vsnprintf(buf, MAXLINE, fmt, ap);
	if(errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s", strerror(error));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}

