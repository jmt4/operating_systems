#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int collatz(int val);

int main(int argc, char *argv[])
{
	pid_t pid;

	if (argc != 2) {
		printf("Expected command line argument -- positive integer\n");
		exit(1);	
	}

	int n = atoi(argv[1]);
	if (n < 0) {
		printf("Input not a positive integer\n");
		exit(1);
	}

	pid = fork();
	if (pid == 0) {
		printf("Child computing ... ");
		while (n > 1) {
			printf("%d ", n);
			n = collatz(n);
		}
		printf("%d ", n);
		printf("\n");
	}
	else {
	/* Wait for child to finish Collatz Conjecture algorithm */
		printf("Parent waiting ... \n");
		wait(NULL);
	}
	return 0;
}

int collatz(int val)
{
	if (val % 2 == 0)
		return val / 2;
	else
		return 3 * val + 1;
}
