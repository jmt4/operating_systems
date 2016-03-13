#include <unistd.h>
#include <stdio.h>
#include "../data-structures/bitvector/bitvector.h"

#define MIN_PID 300
#define MAX_PID 5000

unsigned char* b;

int main(void)
{
	printf("WORDSIZE: %d\n", WORDSIZE);
	allocate_vector(&b, MAX_PID);
	printf("%d", b[0]);
	destroy_vector(&b);
	return 0;
}

int allocate_map(void)
{
	return allocate_vector(&b MAX_PID);
}

int allocate_pid(void)
{
	int i;
	for (i = MIN_PID; i < MAX_PID; i++) {
		if (check_bit(b, i) == 0) return i;	
	}
	return -1;
}

void release_pid(int pid)
{
	destroy_vector(&b);
}
