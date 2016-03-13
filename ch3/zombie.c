#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		printf("Fork Failed");
		return 1;
	}
	else if (pid == 0) {
	/* child process */
	}
	else {
		while (1)
			;
	}
	return 0;
}
