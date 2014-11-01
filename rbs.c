#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "rbs.h"

#define RESULTS_FILE "redblue.txt"
#define ERROR(s) {fprintf(stderr, "error: %s\n", s); exit(EXIT_FAILURE);}

int main(int argc, char *argv[])
{
    int n_procs = 0;
    int board_width = 0;
    int tile_width = 0;
    int max_density = 0;
    int max_steps = 0;
    int random_seed = time(NULL);
    bool interactive = false;

    for (int i = 1; i < argc; i++) {
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
                interactive = true;
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
    board b;
    int num_steps;
    FILE *results_file;
    double elapsed_time = 0;
    init_board(&b, board_width, random_seed);
    for (num_steps = 0; num_steps < max_steps; num_steps++) {
        elapsed_time += check_board(&b, max_density, tile_width, n_procs);
        if (b.complete) {
            break;
        } else {
            elapsed_time += shift_board(&b, n_procs);
        }
    }

    results_file = fopen(RESULTS_FILE, "w");
    print_board(b, results_file);
    for (int i = 1; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
        fprintf(results_file, "%s ", argv[i]);
    }
    fprintf(stdout, "%d %d %.2lf\n", num_steps, b.max_density, elapsed_time);
    fprintf(results_file, "%d %d %.2lf", num_steps, b.max_density, elapsed_time);

    free_board(&b);
    return 0;
}

int rbs_interactive(int argc, char *argv[], int n_procs, int board_width, int tile_width, int max_density,
                    int max_steps, int random_seed)
{
    return 0;
}

