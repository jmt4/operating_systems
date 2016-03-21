#include "util.h"
#include <string.h>

int is_uint(const char *input)
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