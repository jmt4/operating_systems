#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

/* This program will use named pipes for IPC. Unlike parent_child_pipe, we will use two different
 * programs to do the same thing. Process_1 will write a msg to a named pipe (FIFO), process_2 will
 * read it. Interestingly, FIFOs block until another process reads/writes on them. Process two swaps
 * character case, writes msg onto second named pipe, and then process 1 reads and writes to stdout.
*/

int main(void)
{
	char parent_msg[BUFFER_SIZE] = "aM I alOne hERe?";
	char* filename1 = "named_pipes_in.txt";
	char* filename2 = "named_pipes_out.txt";
	int pipes[2];
	int fd;

	pipes[WRITE_END] = mkfifoat(AT_FDCWD, filename1, 0666);
	if (pipes[WRITE_END] == -1) {
		perror("named pipe failed");
		exit(1);
	}

	fd = open(filename1, O_WRONLY);
	write(fd, parent_msg, sizeof parent_msg);
	close(fd);
	
	fd = open(filename2, O_RDONLY);
	char buf;
	while (read(fd, &buf, 1) > 0)
		write(STDOUT_FILENO, &buf, 1);
	write(STDOUT_FILENO, "\n", 1);
	close(fd);
	exit(EXIT_SUCCESS);
}
