#include<stdio.h>
#include<string.h>

static char ctermid_name[L_ctermid];

char *ctermid(char *ptr)
{
	if(ptr == NULL)
		ptr = ctermid_name;

	return (strcpy(ptr, "/dev/tty"));
}

