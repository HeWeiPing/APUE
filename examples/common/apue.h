#ifndef _APUE_H
#define _APUE_H

#define _XOPEN_SOURCE 600  /* Single Unix Specification, Version 3 */

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/termios.h>
#ifndef TIOCGWINSZ
#include<sys/ioctl.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>    /* for offsetof */
#include<string.h>
#include<unistd.h>   /* 包含所有标准Unix函数原型 */
#include<signal.h>

#define MAXLINE 4096
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

/*
 * 信号处理部分
 */
typedef void Sigfunc(int);  /* for signal handler */
#if defined(SIG_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((Sigfunc *)-1)
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

/*
 * Prototypes for functions
 */
char *path_alloc(int *);
long open_max(void);
void clr_fl(int, int);
void set_fl(int, int);
Sigfunc *signal_intr(int, Sigfunc *);

ssize_t readn(int, void *, size_t);
ssize_t writen(int, const void *, size_t);
//为进程打印信号屏蔽字
void pr_mask(const char *);

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_exit(int, const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

void log_msg(const char *, ...);
void log_open(const char *, int, int);
void log_quit(const char *, ...);
void log_ret(const char *, ...);
void log_sys(const char *, ...);

//打印exit状态的说明
void pr_exit(int);

//进程间通信
void TELL_WAIT(void);
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);

//守护进程
void daemonize(const char *);

//记录锁
int lock_reg(int, int, int, off_t, int, off_t);
#define read_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

pid_t lock_test(int, int, off_t, int, off_t);
#define is_read_lockable(fd, offset, whence, len) (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

//进程间通信
int s_pipe(int *);
int send_fd(int, int);
int recv_fd(int, ssize_t (*func)(int, const void *, size_t));
int send_err(int, int, const char *);

int serv_listen(const char *);
int serv_accept(int, uid_t *);
int cli_conn(const char *);

//终端
int tty_cbreak(int);
int tty_raw(int);
int tty_reset(int);
void tty_atexit(void);
#ifdef ECHO
struct termios *tty_termios(void);
#endif

//伪终端
int ptym_open(char *, int);
int ptys_open(char *);
#ifdef TIOCGWINSZ
pid_t pty_fork(int *, char *, int, const struct termios *, const struct winsize *);
#endif
#endif
