#include<stdio.h>
#include<stdlib.h>

typedef void (*Myfunc)(void);

void my_exit1(void)
{
	printf("first exit handler\n");
}

void my_exit2(void)
{
	printf("second exit handler\n");
}

int main(void)
{
    Myfunc exit1 = my_exit1;
	Myfunc exit2 = my_exit2;

	atexit(exit1);
	atexit(exit2);
	atexit(exit2);

	printf("Done\n");
	return 0;
}
