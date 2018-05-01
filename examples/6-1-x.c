#include<pwd.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
	if(argc != 2)
		err_quit("usage: a.out <username>");

	struct passwd *passwdbuf;
	if((passwdbuf = getpwnam(argv[1])) == NULL)
		err_sys("getpwname %s error", argv[1]);

	printf("User: %s\n", passwdbuf->pw_name);
	printf("UID: %d\n", passwdbuf->pw_uid);
	printf("GID: %d\n", passwdbuf->pw_gid);
	printf("Comment: %s\n", passwdbuf->pw_gecos);
	printf("Dir: %s\n", passwdbuf->pw_dir);
	printf("Shell: %s\n", passwdbuf->pw_shell);

	exit(0);
}
