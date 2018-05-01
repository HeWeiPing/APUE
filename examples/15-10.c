#include<stdio.h>
#include<stdlib.h>

#define MAXL 1024

int main(void)
{
	int int1, int2;
	char line[MAXL];

	setvbuf(stdin, NULL, _IOLBF, 0);
	setvbuf(stdout, NULL, _IOLBF, 0);

	while(fgets(line, MAXL, stdin) != NULL) {
		if(sscanf(line, "%d%d", &int1, &int2) == 2) {
			printf("%d\n", int1+int2);
		} else {
			printf("invalid args\n");
		}
	}
	exit(0);
}
