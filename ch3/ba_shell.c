#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE 80 /* maximum length of command */

void split_input(char *commands, int length, char *args[]);
void print_args(char *args[], int num_args);
int skip_space(int index, char *str);
void shift_args(char *args[], int new_args, int max_args);
int count_args(char *commands);
void die(const char *message);

int main(void)
{
	char *args[MAX_LINE/2 + 1]; /* >>> arg1 arg2 arg3 ... */
	int should_run = 1; /* flag for while loop */
	char buffer[MAX_LINE];
	ssize_t bytes;
	char c;
	int i, argcount = 0;
	
	for (i = 0; i < MAX_LINE/2 + 1; i++) {
		args[i] = malloc(sizeof(char) * MAX_LINE);
		memset(args[i], '\0', MAX_LINE);
		if (args[i] == NULL) 
			die("calloc failed");
	}


	while (should_run)
	{

		printf(">>> ");
		fflush(stdout);
		memset(buffer, 0, MAX_LINE);// zero out buffer for each sequence of commands
		bytes = read(STDIN_FILENO, buffer, MAX_LINE - 1);

		if (bytes == MAX_LINE - 1)
			while((c = getchar()) != '\n' && c != EOF)
			/* discard stdin greater than 80 */;
		if (bytes < MAX_LINE - 1) {
			buffer[bytes - 1] = '\0'; //intent is to remove newline
			printf("going after newline in: %s\n", buffer);
		}

		argcount = count_args(buffer);
		printf("number of arguments: %d\n", argcount);
		shift_args(args, argcount, MAX_LINE/2 + 1);

		split_input(buffer, bytes, args);

		print_args(args, 10);
		 /* After reading user input, the steps are: */
		 /* (1) fork a child process using fork() */
		pid_t pid = fork();
		/* (2) the child process will invoke execvp() */
		if(pid == 0) {
			char *execargs[argcount + 1];
			int j;
			for (j = 0; j < argcount; j++)
				execargs[j] = args[j];
			execargs[j + 1] = NULL;
			execvp(execargs[0], execargs);
		}
		/* (3) if command included &, parent will invoke wait */
		else {
		}
	}

	/* free memory */
	for (i = 0;i < MAX_LINE/2 + 1; i++)
		free(args[i]);

	return 0;
}

void split_input(char *commands, int length, char *args[])
{
	/* commands should *always* be null terminated. thus length check unnecessary */
	int i = 0, j = 0, idx = 0;
	while (commands[idx] != '\0') {// "ps -ael\0"	example commands
		if (commands[idx] == ' ') {
			while (commands[++idx] == ' ') ; // burn white space
			
			args[i][j] = '\0';

			if(commands[idx] != '\0')
				i++;
			j = 0;
		} else {
			args[i][j] = commands[idx];
			j++;
			idx++;
		}	
	}
	//args[i + 1] = NULL;
}

void print_args(char *args[], int num_args)
{
	int idx = 0;
	while (idx < num_args) {
		printf("arg%d: %s\n", idx, args[idx]);
		idx++;
	}
}

int count_args(char *commands)
{
	int count = 0, i = 0;
	while (commands[i++] == ' ') ; // burn spaces at beginning, if any
	while (commands[i] != '\0') {
		if (commands[i] == ' ') { // burn consecutive spaces between words
			while (commands[++i] == ' ') ;

			if (commands[i] != '\0') // only if the next char is not end of line do we count
				count++;
		} else {
			i++;
		}
	}
	return count + 1;
}

void shift_args(char *args[], int new_args, int max_args)
{
	char *temp_arr[new_args];
	int i, j, k;
	/* (2) if the new args plus old args exceeds capacity of list, memset oldest and 
	       make them indexed at the beginning
	*/
	for (i = 0; i < new_args; i++)
		/* store address of strings to be overwritten */
		temp_arr[i] = args[max_args - i - 1];
	for (j = max_args - 1; j >= new_args; j--)
		/* shift strings to be kept 'down' */
		args[j] = args[j - new_args];
	for (k = 0; k < new_args; k++)
		/* set beginning addresses to strings to be overwritten */
		args[k] = temp_arr[k];
}

void die(const char *message)
{
	if (errno)
		perror(message);
	else
		printf("%s\n", message);
	exit(1);
}
