#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "board_threaded.h"
#include "orbs.h"
#include "pthread.h"
#include "wallclock.h"

// Use defines for TRUE and FALSE instead of boolean libraries.
#define TRUE 1
#define FALSE 0

// Specify output file for results.
#define RESULTS_FILE "redblue.txt"

// Helper macro for printing error messages and immediately exiting.
#define ERROR(s) {fprintf(stderr, "error: %s\n", s); exit(EXIT_FAILURE);}

/*
 * parse_cmd_args
 * Helper function for parsing args from argv.
 */
void parse_cmd_args(int argc, char *argv[], int *n_procs, int *board_width, int *tile_width, int *max_density,
                    int *max_steps, int *random_seed)
{
    int i;
    for (i = 1; i < argc; i++) {
        switch(argv[i][0]) {
            case 'p':
                *n_procs = atoi(argv[i]+1);
                break;
            case 'b':
                *board_width = atoi(argv[i]+1);
                break;
            case 't':
                *tile_width = atoi(argv[i]+1);
                break;
            case 'c':
                *max_density = atoi(argv[i]+1);
                break;
            case 'm':
                *max_steps = atoi(argv[i]+1);
                break;
            case 's':
                *random_seed = atoi(argv[i]+1);
                break;
            default:
                ERROR("Unrecognized argument.");
        }
    }
    
    if (*n_procs <= 0) ERROR("Thread count must be greater than 0.");
    if (*board_width <= 1) ERROR("Board width must be greater than 1.");
    if (*tile_width <= 0 || *board_width % *tile_width != 0) ERROR("Tile width must divide board width.");
    if (*max_density < 1 || *max_density > 100) ERROR("Max density must be a value between 1 and 100.");
    if (*max_steps <= 0) ERROR("Max steps must be greater than 0.");
}

/*
 * main
 * Run the red blue algorithm.
 *
 * pN: Use N processors. 1 >= N >= 16.
 * bN: Width of board is N points. Board size is N x N.
 * tN: Width of one overlay tile is N. Tile size is N x N. Tile width divides board width.
 * cN: Maximum colour density in integer percent. 1 <= N <= 100.
 * mN: Maximum number of full steps.
 * sN: (optional) Random seed.
 *
 * returns: Exit code.
 */
int main(int argc, char *argv[])
{
    int i;
    int n_procs = 0, board_width = 0, tile_width = 0, max_density = 0, max_steps = 0, random_seed = time(NULL);
    int n_threads, n_check_tasks, n_shift_tasks, num_steps = 0;
    int max_check_thread_tasks, max_shift_thread_tasks;
    double elapsed_time;
    FILE *results_file;
    board b;
    pthread_t *threads;
    check_tiles_threaded_tasks *check_thread_tasks;
    shift_args *shift_thread_tasks;

    StartTime();

    // Parse arguments from command line.
    parse_cmd_args(argc, argv, &n_procs, &board_width, &tile_width, &max_density, &max_steps, &random_seed);

    // Initializ the board.
    init_board(&b, board_width, random_seed);

    // One-time initialization of thread and task memory. 
    // Keeps the program from incurring significant overhead due to malloc/freeing in each shift and check step.
    n_threads = n_procs - 1;
    if (n_threads) {

        // Number of checks is the total number of tiles in the board.
        n_check_tasks = (b.width / tile_width) * (b.width / tile_width);
        max_check_thread_tasks = (n_check_tasks / n_threads) + 1;

        // Number of shifts is the total number of rows or columns in the board.
        n_shift_tasks = b.width;
        max_shift_thread_tasks = (n_shift_tasks / n_threads) + 1;

        // Allocate threads and tasks.
        threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
        check_thread_tasks = (check_tiles_threaded_tasks *) malloc(n_threads * sizeof(check_tiles_threaded_tasks));
        shift_thread_tasks = (shift_args *) malloc(n_threads * sizeof(shift_args));

        // Allocate task fields.
        for (i = 0; i < n_threads; i++) {
            check_thread_tasks[i].args_list = (check_tile_args *) malloc(max_check_thread_tasks * sizeof(check_tile_args));
            check_thread_tasks[i].results = (tile_result *) malloc(max_check_thread_tasks * sizeof(tile_result));
            shift_thread_tasks[i].indices = (int *) malloc(max_shift_thread_tasks * sizeof(int));
        }
    } else {
        threads = NULL;
        check_thread_tasks = NULL;
        shift_thread_tasks = NULL;
    }

    num_steps = orbs(&b, threads, shift_thread_tasks, check_thread_tasks, n_procs, tile_width, max_density, max_steps);

    // Print results to stdout and file.
    results_file = fopen(RESULTS_FILE, "w");
    print_board(b, results_file, FALSE);
    for (i = 1; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
        fprintf(results_file, "%s ", argv[i]);
    }

    // Calculate elasped time after all other operations.
    // Print final lines to stdout and file.
    elapsed_time = EndTime();
    fprintf(stdout, "%d %d %.2lf\n", num_steps, b.max_density, elapsed_time);
    fprintf(results_file, "%d %d %.2lf", num_steps, b.max_density, elapsed_time);

    // Free memory that was allocated if running in parallel.
    if (n_threads) {
        for (i = 0; i < n_threads; i++) {
            free(check_thread_tasks[i].args_list);
            free(check_thread_tasks[i].results);
            free(shift_thread_tasks[i].indices);
        }
        free(shift_thread_tasks);
        free(check_thread_tasks);
        free(threads);
    }

    // Free memory that was allocated in init_board.
    free_board(&b);
    return 0;
}

int orbs(board *b, pthread_t *threads, shift_args *shift_thread_tasks, check_tiles_threaded_tasks * check_thread_tasks,
        int n_procs, int tile_width, int max_density, int max_steps)
{
    int num_steps = 0;
    do {
        shift_board_threaded(b, threads, shift_thread_tasks, n_procs);
        num_steps++;
        check_board_threaded(b, threads, check_thread_tasks, max_density, tile_width, n_procs);
    } while(!b->complete && num_steps < max_steps);
    return num_steps;
}
