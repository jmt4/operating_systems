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

void swap_case(char* ch);

int main(void)
{
	char child_msg[BUFFER_SIZE];
	char* filename1 = "named_pipes_in.txt";
	char* filename2 = "named_pipes_out.txt";
	int pipes[2];
	int fd;

	fd = open(filename1, O_RDONLY);
	char buf;
	int offset = 0;
	while(read(fd, &buf, 1) > 0)
	{
		swap_case(&buf);
		child_msg[offset] = buf;
		if (offset == BUFFER_SIZE - 1) {
			child_msg[offset] = '\0';
			break;
		}
	}
	close(fd);

	pipes[WRITE_END] = mkfifoat(AT_FDCWD, filename2, O_CREAT | O_RDWR);
	if (pipes[WRITE_END] == -1) {
		perror("named pipe failed");
		exit(1);
	}
	fd = open(filename2, O_WRONLY);
	write(fd, child_msg, sizeof child_msg);
	close(pipes[WRITE_END]);
	exit(EXIT_SUCCESS);
}

void swap_case(char* ch)
{
	if (*ch >= 65 && *ch <= 90)
		*ch = *ch + 32;
	else if (*ch >= 97 && *ch <= 122)
		*ch = *ch - 32;
}
