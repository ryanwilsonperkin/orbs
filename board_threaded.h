#ifndef RED_BLUE_BOARD_THREADED_H
#define RED_BLUE_BOARD_THREADED_H

#include "board.h"
#include "pthread.h"

/*
 * check_tile_args
 * Arguments to call check_tile with.
 *
 * b: The board.
 * x_start: The starting column.
 * y_start: The starting row.
 * x_end: The ending column.
 * y_end: The ending row.
 */
typedef struct check_tile_args_
{
    board *b;
    int x_start, x_end;
    int y_start, y_end;
} check_tile_args;

/*
 * check_tiles_threaded_tasks
 * Arguments to pass to check_tiles_threaded.
 *
 * args_list: List of arguments to call check_tile with.
 * results: Results returned from check_tile calls.
 * n_tasks: Number of times to call check_tile.
 */
typedef struct check_tiles_threaded_tasks_
{
    check_tile_args *args_list;
    tile_result *results;
    int n_tasks;
} check_tiles_threaded_tasks;

/*
 * shift_args
 * Arguments to pass to shift_rows_threaded or shift_columns_threaded.
 *
 * b: The board.
 * indices: The indices to call shift_row or shift_column with.
 * n_indices: The number of times to call shift_row or shift_column.
 */
typedef struct shift_args_
{
    board *b;
    int *indices;
    int n_indices;
} shift_args;

/*
 * check_tiles_threaded
 * pthread API threaded function to check tiles.
 *
 * t: pointer to check_tiles_threaded_tasks instance.
 */
void * check_tiles_threaded(void *t);

/*
 * shift_columns_threaded
 * pthread API threaded function to shift columns.
 *
 * t: pointer to shift_args instance.
 */
void * shift_columns_threaded(void * args_);

/*
 * shift_rows_threaded
 * pthread API threaded function to shift rows.
 *
 * t: pointer to shift_args instance.
 */
void * shift_rows_threaded(void * args_);

/*
 * check_board_threaded
 * Check tiles in board for stopping condition using 1 or more threads.
 * Serial if n_procs = 1
 *
 * b: The board.
 * threads: Allocated pthreads objects to task with checking. null if serial.
 * thread_tasks: Tasks to pass to the threads. NULL if serial.
 * max_density: Stopping condition.
 * tile_width: Width of a single tile in the board.
 * n_procs: Number of processors to use.
 */
void check_board_threaded(board *b, pthread_t *threads, check_tiles_threaded_tasks *thread_tasks, int max_density,
                          int tile_width, int n_procs);

/*
 * shift_board_threaded
 * Shift board using 1 or more threads.
 * Serial if n_procs = 1
 *
 * b: The board.
 * threads: Allocated pthreads objects to task with checking. null if serial.
 * thread_tasks: Tasks to pass to the threads. NULL if serial.
 * n_procs: Number of processors to use.
 */ 
void shift_board_threaded(board *b, pthread_t *threads, shift_args *thread_tasks, int n_procs);

/*
 * shift_blue_threaded
 * Shift blue colours in board using 1 or more threads.
 * Serial if n_procs = 1
 *
 * b: The board.
 * threads: Allocated pthreads objects to task with checking. null if serial.
 * thread_tasks: Tasks to pass to the threads. NULL if serial.
 * n_procs: Number of processors to use.
 */
void shift_blue_threaded(board *b, pthread_t *threads, shift_args *thread_tasks, int n_procs);

/*
 * shift_red_threaded
 * Shift red colours in board using 1 or more threads.
 * Serial if n_procs = 1
 *
 * b: The board.
 * threads: Allocated pthreads objects to task with checking. null if serial.
 * thread_tasks: Tasks to pass to the threads. NULL if serial.
 * n_procs: Number of processors to use.
 */
void shift_red_threaded(board *b, pthread_t *threads, shift_args *thread_tasks, int n_procs);

#endif  // RED_BLUE_BOARD_THREADED_H
