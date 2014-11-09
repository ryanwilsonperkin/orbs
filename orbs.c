#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "orbs.h"
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
    int n_procs = 0, board_width = 0, tile_width = 0, max_density = 0, max_steps = 0, random_seed = (int) time(NULL);
    int num_steps = 0;
    double elapsed_time;
    FILE *results_file;
    board b;

    StartTime();

    // Parse arguments from command line.
    parse_cmd_args(argc, argv, &n_procs, &board_width, &tile_width, &max_density, &max_steps, &random_seed);

    // Initializ the board.
    init_board(&b, board_width, random_seed);

    num_steps = orbs(&b, n_procs, tile_width, max_density, max_steps);

    // Print results to stdout and file.
    results_file = fopen(RESULTS_FILE, "w");
    print_board(b, results_file);
    for (i = 1; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
        fprintf(results_file, "%s ", argv[i]);
    }

    // Calculate elasped time after all other operations.
    // Print final lines to stdout and file.
    elapsed_time = EndTime();
    fprintf(stdout, "%d %d %.2lf\n", num_steps, b.max_density, elapsed_time);
    fprintf(results_file, "%d %d %.2lf", num_steps, b.max_density, elapsed_time);

    // Free memory that was allocated in init_board.
    free_board(&b);
    return 0;
}

int orbs(board *b, int n_procs, int tile_width, int max_density, int max_steps)
{
    int num_steps = 0;
    do {
        shift_board(b);
        num_steps++;
        check_board(b, max_density, tile_width);
    } while(!b->complete && num_steps < max_steps);
    return num_steps;
}
