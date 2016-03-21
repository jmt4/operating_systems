#include <errno.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

/* returns 1 if string can be an unsigned integer */
/* envisioned use is something like is_uint(argv[1]) */
int is_uint(const char *input);

