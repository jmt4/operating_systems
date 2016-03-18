#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../util.h"
#include <errno.h>

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NTHREADS 1

struct thread_info {
	pthread_t thread_id;
	int num;
};

void *thread_start(void *params);
void primes_lt_or_eq(int num);

int main(int argc, char *argv[])
{
	int s;
	pthread_attr_t attr;
	struct thread_info *tinfo;

	if (argc != 2)
		handle_error("./threads_prime <some positive number>");

	if (!is_uint(argv[1]))
		handle_error("expecting postive integer as second arg");

	tinfo = calloc(NTHREADS, sizeof(struct thread_info));
	if (tinfo == NULL)
		handle_error("calloc");

	tinfo->num = atoi(argv[1]);

	s = pthread_attr_init(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_init");
	// if we have an array of threads we access like ...
	// &tinfo[].thread_id
	s = pthread_create(&tinfo->thread_id, &attr,
				   &thread_start, tinfo);
	if (s != 0)
		handle_error_en(s, "pthread_create");

	s = pthread_join(tinfo->thread_id, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_join");

	s = pthread_attr_destroy(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_destroy");

	free(tinfo);

	return 0;
}

void *thread_start(void *params)
{
	struct thread_info *tinfo = params;

	primes_lt_or_eq(tinfo->num);

	return NULL;
}

void primes_lt_or_eq(int num)
{
	/* Given a number, this function will print out
	   all prime numbers equal to or less than the 
	   number provided 
		
	   Example
	   ./threads_prime 17
	   2 3 5 7 11 13 17
	   */

	int i, j, count = 1, is_prime;
	int primes[num / 2];
	primes[0] = 2;

	for (i = 3; i < num + 1; i += 2) {
		is_prime = 1;
		for (j = 0; j < count; j++) {
			if (i % primes[j] == 0) {
				is_prime = 0;
				break;
			}
		}
		if (is_prime) {
			primes[count] = i;
			count++;
		}
	}

	for (i = 0; i < count; i++)
		printf("%d ", primes[i]);
	printf("\n");
}