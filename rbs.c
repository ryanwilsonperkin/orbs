#define _CRTDBG_MAP_ALLOC
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <crtdbg.h>

#include "board.h"
#include "board_threaded.h"
#include "rbs.h"
#include "wallclock.h"

#define TRUE 1
#define FALSE 0
#define RESULTS_FILE "redblue.txt"
#define ERROR(s) {fprintf(stderr, "error: %s\n", s); exit(EXIT_FAILURE);}

int main(int argc, char *argv[])
{
    int i;
    int n_procs = 0;
    int board_width = 0;
    int tile_width = 0;
    int max_density = 0;
    int max_steps = 0;
    int random_seed = time(NULL);
    char interactive = FALSE;

    for (i = 1; i < argc; i++) {
        switch(argv[i][0]) {
            case 'p':
                n_procs = atoi(argv[i]+1);
                break;
            case 'b':
                board_width = atoi(argv[i]+1);
                break;
            case 't':
                tile_width = atoi(argv[i]+1);
                break;
            case 'c':
                max_density = atoi(argv[i]+1);
                break;
            case 'm':
                max_steps = atoi(argv[i]+1);
                break;
            case 's':
                random_seed = atoi(argv[i]+1);
                break;
            case 'i':
                interactive = TRUE;
                break;
            default:
                ERROR("Unrecognized argument.");
        }
    }
    
    if (n_procs <= 0) ERROR("Thread count must be greater than 0.");
    if (board_width <= 1) ERROR("Board width must be greater than 1.");
    if (tile_width <= 0 || board_width % tile_width != 0) ERROR("Tile width must divide board width.");
    if (max_density < 1 || max_density > 100) ERROR("Max density must be a value between 1 and 100.");
    if (max_steps <= 0) ERROR("Max steps must be greater than 0.");

    if (interactive) {
        return rbs_interactive(argc, argv, n_procs, board_width, tile_width, max_density, max_steps, random_seed);
    } else {
        return rbs(argc, argv, n_procs, board_width, tile_width, max_density, max_steps, random_seed);
    }
    return 0;
}

int rbs(int argc, char *argv[], int n_procs, int board_width, int tile_width, int max_density, int max_steps,
        int random_seed)
{
    int i;
    board b;
    FILE *results_file;
    pthread_t *threads;
    int n_threads, num_steps = 0;
    double elapsed_time;

    StartTime();
    // Initialization of board, threads, and memory.
    n_threads = n_procs - 1;
    if (n_threads) {
        threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
    } else {
        threads = NULL;
    }

    init_board(&b, board_width, random_seed);
    do {
        shift_board_threaded(&b, n_procs);
        num_steps++;
        check_board_threaded(&b, threads, max_density, tile_width, n_procs);
    } while(!b.complete && num_steps < max_steps);
    elapsed_time = EndTime();

    results_file = fopen(RESULTS_FILE, "w");
    print_board(b, results_file);
    for (i = 1; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
        fprintf(results_file, "%s ", argv[i]);
    }
    fprintf(stdout, "%d %d %.2lf\n", num_steps, b.max_density, elapsed_time);
    fprintf(results_file, "%d %d %.2lf", num_steps, b.max_density, elapsed_time);

    free_board(&b);
    free(threads);
    _CrtDumpMemoryLeaks();
    return 0;
}

int rbs_interactive(int argc, char *argv[], int n_procs, int board_width, int tile_width, int max_density,
                    int max_steps, int random_seed)
{
    return 0;
}

