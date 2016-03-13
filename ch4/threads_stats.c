#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#define NTHREADS 3

void die(char *message);
void *thread_start(void *params);
int t_avg(int *seq, int argc);
int t_min(int *seq, int argc);
int t_max(int *seq, int argc);

int statistics[NTHREADS]; /* index 0:avg value, 1:min value, 2:max value */

struct thread_data {	 /* used as argument in pthread_create() */
	pthread_t thread_id; /* ID of thread stored when pthread_create() is called */
	int argc;			 /* number of command line arguments minus 1 */
	int *args;			 /* pointer to an sequence (array) of numbers from command line */
	int tid;
};

int main(int argc, char *argv[])
{
	/* A series of numbers is passed through the command line */
	/* Worker threads perform some statistics on these numbers */
	int i, s, seq[argc - 1];
	struct thread_data *tdata;
	pthread_attr_t attr;

	if (argc < 2)
		die("expecting sequence of numbers <a1 a2 a3 a4 a5 .. an>");

	s = pthread_attr_init(&attr);
	if (s != 0)
		die("Error initializing pthread_attr_t object");
	
	/* convert command line argument to numbers */
	for (i = 0; i < argc - 1; i++) {
		seq[i] = atoi(argv[i + 1]);
	}

	tdata = calloc(NTHREADS, sizeof(struct thread_data));
	if (tdata == NULL) die("calloc");

	/* create threads and assign runner functions */
	for (i = 0; i < NTHREADS; i++) {
		tdata[i].argc = argc - 1;
		tdata[i].args = seq;
		tdata[i].tid = i;
		s = pthread_create(&tdata[i].thread_id, &attr, 
						   &thread_start, &tdata[i]);
		if (s != 0)
			die("pthread_create");
	}

	if (pthread_attr_destroy(&attr) != 0)
		die("Error destroying pthread_attr_t object");

	for (i = 0; i < NTHREADS; i++) {
		printf("Joining threads...\n");
		s = pthread_join(tdata[i].thread_id, NULL);
		//s = pthread_join((&tdata[i])->thread_id, NULL);
		if (s) die("pthread_join");
	}

	free(tdata);
	printf("avg: %d\nmin: %d\nmax: %d\n", statistics[0], statistics[1], statistics[2]);

	return 0;
}

void *thread_start(void *params)
{
	struct thread_data* tdata = params;
	if (tdata->tid == 0) {
		printf("Thread %d here\n", tdata->tid);
		statistics[tdata->tid] = t_avg(tdata->args, tdata->argc);
	} else if (tdata->tid == 1) {
		printf("Thread %d here\n", tdata->tid);
		statistics[tdata->tid] = t_min(tdata->args, tdata->argc);
	} else {
		printf("Thread %d here\n", tdata->tid);
		statistics[tdata->tid] = t_max(tdata->args, tdata->argc);
	}

	return NULL;
}

int t_avg(int *seq, int argc)
{
	int i, sum = 0;
	for (i = 0; i < argc; i++) {
		sum += seq[i];
	}
	return sum / argc;
}

int t_min(int *seq, int argc)
{
	int i, min = seq[0];
	for (i = 1; i < argc; i++) {
		if (seq[i] < min)
			min = seq[i];
	}
	return min;
}

int t_max(int *seq, int argc)
{
	int i, max = seq[0];
	for (i = 1; i < argc; i++) {
		if (seq[i] > max)
			max = seq[i];
	}
	return max;
}

void die(char *message)
{
	if (errno) perror(message);
	else printf("%s\n", message);
	exit(1);
}