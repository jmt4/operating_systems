#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXMSGSIZE 100
#define MAXBUFFSIZE 10

int main(int argc, char *argv[])
{
	char src[40], dest[40];
	char src_prompt[] = {"Enter name of source file\n"};
	char dest_prompt[] = {"Enter name of destination file\n"};
	int fd_src, fd_dest; // File descriptors for source and destination
	char buffer[10]; // Buffer for copying files
	ssize_t bytes;
	
	write(STDOUT_FILENO, (void *)src_prompt, sizeof src_prompt);
	bytes = read(STDIN_FILENO, (void *)src, 40);
	src[bytes - 1] = '\0'; // new line char is read!!!!

	write(STDOUT_FILENO, (void *)dest_prompt, sizeof dest_prompt);
	bytes = read(STDIN_FILENO, (void *)dest, 40);
	dest[bytes - 1] = '\0';
	
	errno = 0;
	fd_src = open(src, O_RDONLY);
	if (fd_src == -1) {
		perror("open() source error");
		exit(1);
	}
	fd_dest = open(dest, O_APPEND|O_CREAT|O_WRONLY, 0666);
	if (fd_dest < 0) {
		perror("open() dest error");
		exit(1);
	}

	while(1) {
		bytes = read(fd_src, (void *)buffer, MAXBUFFSIZE);
		if (bytes < 0) {
			perror("read error");
			break;
		}
		if (bytes == 0) {
			perror("EOF");
			break;
		}
		bytes = write(fd_dest, (void *)buffer, bytes);
		if (bytes < 0) {
			perror("write() error");
			break;
		}
	}
	close(fd_src);
	close(fd_dest);
	return 0;
}
