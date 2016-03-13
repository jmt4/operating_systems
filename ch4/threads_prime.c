#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define error_handler_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define error_handler(msg) \
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
	pthread_attr_t attr;
	struct thread_info *tinfo;

	primes_lt_or_eq(2);
	//tinfo = calloc(NTHREADS, sizeof(struct thread_info));
	// if we have an array of threads we access like ...
	// &tinfo[].thread_id
	//pthread_create(&tinfo->thread_id, &attr,
	//			   &thread_start, tinfo);
}

void *thread_start(void *params)
{
	struct thread_info *tinfo = params;

	//primes_lt_or_eq();

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
}