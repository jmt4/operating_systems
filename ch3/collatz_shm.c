#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

int collatz(int val);

int main(int argc, char *argv[])
{
	/* size of shm_object */
	size_t page_size = (size_t) sysconf(_SC_PAGESIZE);
	/* name of shm object */
	const char* name = "/shm_collatz";
	pid_t pid;
	/* file descriptor for mem mapped file*/
	int shm_fd_1, shm_fd_2;
	/* pointer to shm object */
	void *ptr_1, *ptr_2;
 
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
		int l;
		shm_fd_1 = shm_open(name, O_CREAT | O_RDWR, 0666);
		if (shm_fd_1 == -1) {
			perror("error shm_open()");
			exit(1);
		}
		ftruncate(shm_fd_1, page_size);
		ptr_1 = mmap(NULL, page_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd_1, 0);
		if (ptr_1 == MAP_FAILED) {
			perror("child: mmap failure");
			exit(1);
		}

		printf("Child computing ... \n");

		while (n > 1) {
			l = sprintf(ptr_1, "%d ", n);
			ptr_1 += l; // overwrite null byte
			n = collatz(n);
		}
		l = sprintf(ptr_1, "%d", n);
		ptr_1 += l; // dont overwrite for last append
	}
	else {
	/* Wait for child to finish Collatz Conjecture algorithm */
		printf("Parent waiting ... \n");
		wait(NULL);
		shm_fd_2 = shm_open(name, O_RDONLY, 0666);
		if (shm_fd_2 == -1) {
			perror("error shm_open()");
			exit(1);
		}
		ptr_2 = mmap(NULL, page_size, PROT_READ, MAP_SHARED, shm_fd_2, 0);
		if (ptr_2 == MAP_FAILED) {
			perror("parent: mmap failure");
			exit(1);
		}
		printf("Parent reading from shared memory object\n");
		printf("%s\n", (char*)ptr_2);
		shm_unlink(name);
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
