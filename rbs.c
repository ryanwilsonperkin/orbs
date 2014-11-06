#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "board_threaded.h"
#include "rbs.h"
#include "pthread.h"
#include "wallclock.h"

#define TRUE 1
#define FALSE 0
#define RESULTS_FILE "redblue.txt"
#define ERROR(s) {fprintf(stderr, "error: %s\n", s); exit(EXIT_FAILURE);}

void parse_cmd_args(int argc, char *argv[], int *n_procs, int *board_width, int *tile_width, int *max_density, int *max_steps, int *random_seed, char *interactive)
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
            case 'i':
                *interactive = TRUE;
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

int main(int argc, char *argv[])
{
    int i;
    int n_procs = 0, board_width = 0, tile_width = 0, max_density = 0, max_steps = 0, random_seed = time(NULL);
    char interactive = FALSE;
    int n_threads, n_check_tasks, n_shift_tasks, num_steps = 0;
    int max_check_thread_tasks, max_shift_thread_tasks;
    double elapsed_time;
    board b;
    FILE *results_file;
    pthread_t *threads;
    check_tiles_threaded_tasks *check_thread_tasks;
    shift_args *shift_thread_tasks;

    StartTime();
    parse_cmd_args(argc, argv, &n_procs, &board_width, &tile_width, &max_density, &max_steps, &random_seed, &interactive);
    // Initialization of board, threads, and memory.
    init_board(&b, board_width, random_seed);
    n_threads = n_procs - 1;
    if (n_threads) {
        n_check_tasks = (b.width - tile_width) * (b.width - tile_width);
        n_shift_tasks = b.width;
        max_check_thread_tasks = (n_check_tasks / n_threads) + 1;
        max_shift_thread_tasks = (n_shift_tasks / n_threads) + 1;
        threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
        check_thread_tasks = (check_tiles_threaded_tasks *) malloc(n_threads * sizeof(check_tiles_threaded_tasks));
        shift_thread_tasks = (shift_args *) malloc(n_threads * sizeof(shift_args));
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

    if (interactive) {
        num_steps = rbs_interactive(&b, threads, shift_thread_tasks, check_thread_tasks, n_procs, tile_width, max_density, max_steps);
    } else {
        num_steps = rbs(&b, threads, shift_thread_tasks, check_thread_tasks, n_procs, tile_width, max_density, max_steps);
    }

    results_file = fopen(RESULTS_FILE, "w");
    print_board(b, results_file, FALSE);
    for (i = 1; i < argc; i++) {
        fprintf(stdout, "%s ", argv[i]);
        fprintf(results_file, "%s ", argv[i]);
    }
    elapsed_time = EndTime();
    fprintf(stdout, "%d %d %.2lf\n", num_steps, b.max_density, elapsed_time);
    fprintf(results_file, "%d %d %.2lf", num_steps, b.max_density, elapsed_time);

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
    free_board(&b);
    return 0;
}

int rbs(board *b, pthread_t *threads, shift_args *shift_thread_tasks, check_tiles_threaded_tasks * check_thread_tasks, int n_procs, int tile_width, int max_density, int max_steps)
{
    int num_steps = 0;
    do {
        shift_board_threaded(b, threads, shift_thread_tasks, n_procs);
        num_steps++;
        check_board_threaded(b, threads, check_thread_tasks, max_density, tile_width, n_procs);
    } while(!b->complete && num_steps < max_steps);
    return num_steps;
}

int rbs_interactive(board *b, pthread_t *threads, shift_args *shift_thread_tasks, check_tiles_threaded_tasks * check_thread_tasks, int n_procs, int tile_width, int max_density, int max_steps)
{
    char c;
    int additional_steps, result;
    int n_steps = 0, n_half_steps = 0;
    printf("rbs: Interactive mode.\n");
    do {
        c = getchar();
        if (c == '\n') {
            shift_board_threaded(b, threads, shift_thread_tasks, n_procs);
            n_half_steps += 2;
            n_steps++;
        } else if (c == '#') {
            scanf("%d", &additional_steps);
            result = rbs(b, threads, shift_thread_tasks, check_thread_tasks, n_procs, tile_width, max_density, additional_steps);
            n_half_steps += 2 * result;
            n_steps += result;
            getchar();
        } else if (c == 'h') {
            if (n_half_steps % 2 == 0) {
                shift_red_threaded(b, threads, shift_thread_tasks, n_procs);
            } else {
                shift_blue_threaded(b, threads, shift_thread_tasks, n_procs);
                n_steps++;
            }
            n_half_steps++;
            getchar();
        } else if (c == 'c') {
            if (n_half_steps % 2 == 1) {
                shift_blue_threaded(b, threads, shift_thread_tasks, n_procs);
                n_half_steps++;
                n_steps++;
            }
            return n_steps + rbs(b, threads, shift_thread_tasks, check_thread_tasks, n_procs, tile_width, max_density, max_steps - n_steps);
        } else if (c == 'x') {
            break;
        } else {
            printf("Invalid character.\n");
        }
        check_board_threaded(b, threads, check_thread_tasks, max_density, tile_width, n_procs);
        print_board(*b, stdout, TRUE);
    } while (!b->complete && n_steps < max_steps);
    return n_steps;
    return 0;
}
