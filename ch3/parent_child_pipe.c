#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

void swap_case(char* ch);

/* This program demonstrates IPC using ordinary pipes with a parent-child 
 * relationship between the processes. The parent process will send a string
 * to the child, the child will reverse the case, and send it back to the
 * parent who will then print it.
**/

int main(void)
{
	char parent_msg[BUFFER_SIZE] = "aM I alOne hERe?";
	char child_msg[BUFFER_SIZE];
	int pipe_one[2]; // pipe 1 is to send/recv parent/child
	int pipe_two[2]; // pipe 2 is to send/recv child/parent
	pid_t pid;

	if (pipe(pipe_one) == -1) {
		perror("pipe_one failed");
		exit(EXIT_FAILURE);
	}
	if (pipe(pipe_two) == -1) {
		perror("pipe_two failed");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
	/* child here */
		/* close unused end*/
		close(pipe_one[WRITE_END]);
		/* start reading from pipe_one */
		char buf;
		int offset = 0;
		while (read(pipe_one[READ_END], &buf, 1) > 0)
		{
			swap_case(&buf);
			child_msg[offset] = buf;
			offset++;
			if (offset == BUFFER_SIZE - 1) {
				child_msg[offset] = '\0';
				break; //buffer overflow
			}
		}
		close(pipe_one[READ_END]);

		/* close unused read end of pipe_two */
		close(pipe_two[READ_END]);
		/* write to child msg */
		write(pipe_two[WRITE_END], child_msg, sizeof child_msg);
		close(pipe_two[WRITE_END]);
		_exit(EXIT_SUCCESS);
	}
	else {
	/* parent */
		/* close unused end */
		close(pipe_one[READ_END]);
		/* write to pipe */
		write(pipe_one[WRITE_END], parent_msg, sizeof parent_msg);
		close(pipe_one[WRITE_END]);
		/* wait for child to modify and send back */
		wait(NULL);
		/* close unused end of pipefd_two */
		close(pipe_two[WRITE_END]);
		/* write message to stdout */
		char buf;
		while (read(pipe_two[READ_END], &buf, 1) > 0) // lots of system calls
			write(STDOUT_FILENO, &buf, 1);
		write(STDOUT_FILENO, '\n', 1);
		close(pipe_two[READ_END]);
		exit(EXIT_SUCCESS);
	}
}

void swap_case(char* ch)
{
	if (*ch >= 65 && *ch <= 90)
		*ch = *ch + 32;
	else if (*ch >= 97 && *ch <= 122)
		*ch = *ch - 32;
}
