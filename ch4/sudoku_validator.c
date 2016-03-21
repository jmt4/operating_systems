#include <stdlib.h>
#include <stdio.h>
#include "../util.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ROWS 9
#define COLUMNS 9
#define NUMTHREADS 11

struct thread_info {
	pthread_t thread_id;
	int tid;
	int region_valid;
	const char *region_type;
	int *sudoku[ROWS];
};

/* for reporting errors in sudoku puzzles */
static const char *region_types[] = {"Row", "Column", "Box"};

/*
 * The purpose of this program is to validate a given sudoku
 * solution using multiple threads. We will use the suggested
 * approach of the book. That is, use one thread to validate
 * the columns of the puzzle, a second thread to validate the
 * rows of the puzzle, and 9 threads to validate each 3 x 3
 * box of the puzzle for a total of 11 threads. Three valid
 * and three invalid sudoku test cases are provided.

 4 1 7 |3 6 9 |8 2 5
 6 3 2 |1 5 8 |9 4 7
 9 5 8 |7 2 4 |3 1 6
 ------+------+-----
 8 2 5 |4 3 7 |1 6 9
 7 9 1 |5 8 6 |4 3 2
 3 4 6 |9 1 2 |7 5 8
 ------+------+-----
 2 8 9 |6 4 3 |5 7 1
 5 7 3 |2 9 1 |6 8 4
 1 6 4 |8 7 5 |2 9 3

Rows Offset:		9 * i + i
Column Offset:		9 * i
Grid Offset:		See validate_grid()
 */

// forward declarations
void load_puzzle(int *puzzle[], char *data, int bytes_read);
void print_puzzle(int *puzzle[]);
void thread_info_init(struct thread_info *tinfo, int tid, int *puzzle[]);
void thread_info_print(struct thread_info *tinfo);
void *thread_start(void *params);
void validate_rows(struct thread_info *tinfo);
void validate_columns(struct thread_info *tinfo);
void validate_grid(struct thread_info *tinfo);

int main(int argc, char *argv[])
{
	int s, i, fd, *puzzle[ROWS];
	char buff[4096];
	struct thread_info *tinfo[NUMTHREADS];
	pthread_attr_t attr;

	if (argc != 2)
		handle_error("Usage ./sudoku_validator <sudoke file name>");

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		handle_error_en(s, "open");

	s = read(fd, buff, sizeof(buff));
	if (s == -1)
		handle_error_en(s, "read");

	s  = pthread_attr_init(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_init");

	/* allocate memory for sudoku puzzle */
	for (i = 0; i < ROWS; i++) {
		puzzle[i] = malloc(sizeof(int) * COLUMNS);
		if (puzzle[i] == NULL)
			handle_error("puzzle memory");
	}

	/* load puzzle into an array on points for thread use */
	load_puzzle(puzzle, buff, s);
	//print_puzzle(puzzle);

	/* allocate memory and initialize thread structs */
	for (i = 0; i < NUMTHREADS; i++) {
		tinfo[i] = malloc(sizeof(*tinfo[i]));
		if (tinfo == NULL)
			handle_error("thread_info memory");
		thread_info_init(tinfo[i], i, puzzle);
		//thread_info_print(tinfo[i]);
	}

	/* create and put threads to work */
	for (i = 0; i < NUMTHREADS; i++) {
		s = pthread_create(&(tinfo[i]->thread_id), &attr,
			&thread_start, tinfo[i]);
		if (s != 0)
			handle_error_en(s, "pthread_create");
	}

	for (i = 0; i < NUMTHREADS; i++) {
		s = pthread_join(tinfo[i]->thread_id, NULL);
		if (s != 0)
			handle_error_en(s, "pthread_join");
	}

	s = close(fd);
	if (s == -1)
		handle_error_en(s, "close");

	for (i = 0; i < NUMTHREADS; i++) {
		if (tinfo[i]->region_valid)
			thread_info_print(tinfo[i]);
	}
	
	for (i = 0; i < NUMTHREADS; i++)
		free(tinfo[i]);

	/* free sudoku puzzle */
	for (i = 0; i < ROWS; i++)
		free(puzzle[i]);

	return 0;
}

void thread_info_print(struct thread_info *tinfo)
{
	printf("Thread ID: %3d\tPuzzle Region %10s Valid: %3d\n",
	       tinfo->tid, tinfo->region_type, tinfo->region_valid);
}

void thread_info_init(struct thread_info *tinfo, int tid, int *puzzle[])
{
	int i;
	tinfo->tid = tid;
	tinfo->region_valid = 0;
	for (i = 0; i < ROWS; i++)
		tinfo->sudoku[i] = puzzle[i];

	if (tid == 0)
		/* row */
		tinfo->region_type = region_types[0];
	else if (tid == 1)
		/* column */
		tinfo->region_type = region_types[1];
	else
		/* box */
		tinfo->region_type = region_types[2];
}

void print_puzzle(int *puzzle[])
{
	int i, j;
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			printf("%d ", puzzle[i][j]);
		}
		printf("\n");
	}
}

void load_puzzle(int *puzzle[], char *data, int bytes_read)
{
	int i, j, pos = 0;
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLUMNS; j++) {
			puzzle[i][j] = atoi(&data[pos]);
			pos += 2;
		}
}

void validate_rows(struct thread_info *tinfo)
{
	int i, j, value;
	for (i = 0; i < ROWS; i++) {
		int valid[ROWS] = {0};
		for (j = 0; j < COLUMNS; j++) {
			value = tinfo->sudoku[i][j];
			if (!valid[value - 1]) {
				valid[value - 1] = 1;
			} else {
				/* break if even one row is not valid */
				tinfo->region_valid = i + 1;
				return;
			}
		}
	}
}

void validate_columns(struct thread_info *tinfo)
{
	int i, j, value;
	for (i = 0; i < COLUMNS; i++) {
		int valid[COLUMNS] = {0};
		for (j = 0; j < ROWS; j++) {
			value = tinfo->sudoku[j][i];
			if (!valid[value - 1]) {
				valid[value - 1] = 1;
			} else {
				/* break if even one row is not valid */
				tinfo->region_valid = i + 1;
				return;
			}
		}
	}
}

void validate_grid(struct thread_info *tinfo)
{
	// this gives us numbers 0, 1, 2
	int x = (tinfo->tid - 2) / 3;
	// this gives us 0, 1, 2
	int y = (tinfo->tid - 2) % 3;
	/* cartesian product gives 
	 * {(0,0),(0,1),(0,2),(1,0),(2,0),(1,1),(1,2),(2,1),(2,2)}
	 * (0,0)-> x:[0,2];y:[0,2]

	     1   |   2   |   3
	   (0,0) | (1,0) | (2,0)
	         |       |
	  -------+-------+-------
	     4   |   5   |   6
	   (0,1) | (1,1) | (2,1)
	         |       |
	  -------+-------+-------
	      7  |   8    |  9
	   (0,2) | (1,2) | (2,2)
	         |       |

	  So we can use x + 3y + 1 to assign a grid number
	  if grids are labeled 1 - 9 left to right
	 */
	int i, j, value;
	int valid[ROWS] = {0};
	for (i = 3 * x; i < 3 * x + 3; i++) {
		for (j = 3 * y; j < 3 * y + 3; j++) {
			value = tinfo->sudoku[j][i];
			if (!valid[value - 1]) {
				valid[value - 1] = 1;
			} else {
				tinfo->region_valid = tinfo->tid + 1;
				return;
			}
		}
	}
}

void *thread_start(void *params)
{
	struct thread_info *tinfo = params;
	if (tinfo->tid == 0)
		validate_rows(tinfo);
	else if (tinfo->tid == 1)
		validate_columns(tinfo);
	else if (tinfo->tid == 3)
		validate_grid(tinfo);
	return NULL;
}