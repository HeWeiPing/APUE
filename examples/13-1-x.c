#include "apue.h"
#include<termios.h>

/*
 * test the function: daemonize()
 * you may first need to decomment several code in this function first
 */

void pr_ids(char *ident)
{
	printf("%s: pid = %d, ppid = %d, pgrp = %d, tpgrp = %d, sid = %d\n", 
			ident, getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_FILENO), tcgetsid(STDIN_FILENO));

	fflush(stdout);
}

int main(void)
{
	const char *cmd = "Cmd_Test";
    pr_ids("before daemonize");
	daemonize(cmd);
}
