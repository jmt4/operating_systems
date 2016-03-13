#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 16
#define READ_END 0
#define WRITE_END 1

void die(const char* message);


int main(int argc, char* argv[])
{
	if(argc < 3) {
		die("USAGE: ./filecopy <source> <destination>");
	}
	pid_t pid;
	int fd[2];

	if(pipe(fd) == -1) {
		die("Pipe error");
	}

	pid = fork();
	if(pid == -1) {
		die("Fork failed");
	}

	if(pid == 0) /* child process */
	{
		/* close unused end */
		close(fd[WRITE_END]);
		
		char buff[BUFFER_SIZE];
		int dest = open(argv[2], O_CREAT | O_WRONLY, 0666);
		if(dest == -1) {
			die("Error opening dest");
		}
		int bytes;
		while((bytes = read(fd[READ_END], buff, sizeof(buff))) > 0)
			write(dest, buff, sizeof(buff));
		close(fd[READ_END]);
		close(dest);
	}
	else /* parent process */
	{
		/* close unused end */
		close(fd[READ_END]);
		int src = open(argv[1], O_RDONLY, 0666);
		if(src == -1) {
			die("Error opening src");
		}

		char buff[BUFFER_SIZE];
		int bytes;
		while((bytes = read(src, buff, sizeof(buff))) > 0)
			write(fd[WRITE_END], buff, bytes);
		close(fd[WRITE_END]);
		wait(NULL);
	}

	return 0;
}

void die(const char* message)
{
	if(errno) {
		perror(message);
	}
	else {
		printf("%s\n", message);
	}

	exit(1);
}
	}

	exit