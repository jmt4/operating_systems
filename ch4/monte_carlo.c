#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/random.h>

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NTHREADS 1

struct thread_info {		/* info a thread will need */
	pthread_t thread_id;
	int randc;				/* random number count */
};

// count points inside circle
double count = 0.0;

// forward declarations
void *thread_start(void *params);
void seed_rand48(unsigned short *seed);
int is_digit(const char *input);

int main(int argc, char *argv[])
{
	int s;
	struct thread_info *tinfo;
	pthread_attr_t attr;

	if (argc != 2)
		handle_error("expecting ./monte_carlo <positive integer>");

	if (!is_digit(argv[1]))
		handle_error("positive integer not given");

	tinfo = calloc(NTHREADS, sizeof(struct thread_info));
	tinfo->randc = atoi(argv[1]);

	s = pthread_attr_init(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_init");

	s = pthread_create(&tinfo->thread_id, &attr,
					   &thread_start, tinfo);
	//printf("%p %p\n", tinfo, &tinfo[0]);

	if (s != 0)
		handle_error_en(s, "pthread_create");

	s = pthread_attr_destroy(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_destroy");

	s = pthread_join(tinfo->thread_id, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_join");

	printf("%f\n", 4.0 * (count / tinfo->randc));

	return 0;
}

void *thread_start(void *params)
{
	int i;
	double x, y;
	unsigned short seed[3];
	struct thread_info *tinfo = params;
	
	/* for erand48 */
	seed_rand48(seed);

	for (i = 0; i < tinfo->randc; i++) {
		x = (2 * erand48(seed)) - 1;
		y = (2 * erand48(seed)) - 1;
		if (sqrt(x * x + y * y) < 1.0000000000000)
			count++;
	}

	return NULL;
}

void seed_rand48(unsigned short *seed)
{
	int s, random_data = open("/dev/urandom", O_RDONLY);
	char ch[6];

	/* read from Linux random number devices */
	/* blocks if enough random data does not exist */
	s = read(random_data, &ch, 6);
	if (s < 0)
		handle_error("read");

	/* we need 2 bytes per slot to seed erand, so ... */
	/* read random byte, ssl, read random byte and logical or */
	/* guaranteed overkill for this simple exercise but ... */
	seed[0] = (ch[0] << 8) | ch[1];
	seed[1] = (ch[2] << 8) | ch[3];
	seed[2] = (ch[4] << 8) | ch[5];
	
	close(random_data);
}

int is_digit(const char *input)
{
	/* if string is a number, return nonzero */
	int i = strlen(input);
	if (i == 0) return 0;
	while (*input != '\0')
		if (*input >= 48 && *input <= 57)
			input++;
		else
			return 0;
	return 1;
}