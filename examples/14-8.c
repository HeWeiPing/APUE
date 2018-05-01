#include<stropts.h>
#include<fcntl.h>
#include<unistd.h>

extern err_ret(const char *, ...);
extern err_msg(const char *, ...);

int main(int argc, char **argv)
{
	int i, fd;
	for(i=1; i<argc; i++) {
		if((fd = open(argv[i], O_RDONLY)) < 0) {
			err_ret("%s: can't open", argv[i]);
			continue;
		}

		if(isastream(fd))
			err_ret("%s: not a stream", argv[i]);
		else
			err_msg("%s: streams device", argv[i]);
	}
}

