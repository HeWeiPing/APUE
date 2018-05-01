#include "apue.h"

/*
 * return a STREAMS-based pipe
 */
int s_pipe(int fd[2])
{
	return (pipe(fd));
}
