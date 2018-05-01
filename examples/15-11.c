#include<unistd.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>

#define ARRAY_SIZE 40000
#define MALLOC_SIZE 100000
#define SHM_SIZE 100000
#define SHM_MODE 0600

char array[ARRAY_SIZE];

int main(void)
{
	int shmid;
	char *ptr, *shmptr;

	printf("array[] from %lx to %lx\n", (unsigned long)&array[0], 
			(unsigned long)&array[ARRAY_SIZE]);
	printf("stack around %lx\n", (unsigned long)&shmid);

	ptr = malloc(MALLOC_SIZE);
	printf("malloced from %lx to %lx\n", (unsigned long)ptr, 
			(unsigned long)ptr+MALLOC_SIZE);

	shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE);
	shmptr = shmat(shmid, 0, 0);
	printf("shared memory attached from %p to %lx\n", shmptr, 
			(unsigned long)shmptr+SHM_SIZE);

	shmctl(shmid, IPC_RMID, 0);

	exit(0);
}
