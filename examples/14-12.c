#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	int fdin, fdout;
	void *src, *dst;
	struct stat statbuf;

	if(argc != 3) {
		fprintf(stderr, "usage: %s <from> <to>", argv[0]);
		exit(1);
	}

	fdin = open(argv[1], O_RDONLY);
	fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	fstat(fdin, &statbuf);
    printf("source file length: %d\n", statbuf.st_size);
	//ftruncate(fdout, statbuf.st_size);
	ftruncate(fdout, 6);

	//memory mapping
	src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
	dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);

	memcpy(dst, src, statbuf.st_size);
    msync(dst, statbuf.st_size, MS_SYNC);

	fdout = open(argv[2], O_RDONLY);
	fstat(fdout, &statbuf);
	printf("desti file length: %d\n", statbuf.st_size);

	exit(0);
}

