#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "../util.h"
#include <pthread.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

/*
 * On the command line, the user will enter the number of fibonacci
 * numbers to compute. The main thread of execution will pass this
 * data to another thread which will calculate and store the values.
 * The main thread waits for child thread to finish then outputs the 
 * sequence of numbers.
 */

 /* Integer overflow for fibonacci numbers past the 94th */

struct thread_info {
	pthread_t thread_id;
	int num;
	unsigned long *fib_nums;
};

// forward declarations
void *thread_start(void *params);

int main(int argc, char *argv[])
{
	int s, i;
	struct thread_info *tinfo;
	pthread_attr_t attr;

	if (argc != 2)
		handle_error("Usage: ./thread_fibo <some positive number>");
	if (!is_uint(argv[1]))
		handle_error("Usage: ./thread_fibo <some positive number>");

	tinfo = malloc(sizeof(*tinfo));
	if (tinfo == NULL)
		handle_error("Memory error");

	tinfo->num = atoi(argv[1]);
	tinfo->fib_nums = malloc(sizeof(unsigned long) * tinfo->num);
	
	s = pthread_attr_init(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_init");
	s = pthread_create(&tinfo->thread_id, &attr,
		&thread_start, tinfo);
	if (s != 0)
		handle_error_en(s, "pthread_create");
	s = pthread_join(tinfo->thread_id, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_join");

	/* Output Fibonacci numbers */
	for (i = 0; i < tinfo->num; i++) {
		printf("%lu\n", tinfo->fib_nums[i]);
	}
	free(tinfo->fib_nums);
	free(tinfo);

	return 0;
}

void *thread_start(void *params)
{
	struct thread_info *tinfo = params;
	unsigned long a = 0, b = 1;
	int count;

	if (tinfo->num == 1) {
		tinfo->fib_nums[0] = a;
	} else {
		tinfo->fib_nums[0] = a;
		tinfo->fib_nums[1] = b;
		count = 2;
		while (count < tinfo->num) {
			tinfo->fib_nums[count] = a + b;
			a = b;
			b = tinfo->fib_nums[count];
			count++;
		}
	}
	return NULL;
}