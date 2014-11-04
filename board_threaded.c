#include <pthread.h>
#include <stdlib.h>
#include <windows.h>

#include "board_threaded.h"
#include "board.h"

typedef struct check_tile_args_
{
    board *b;
    int x_start, x_end;
    int y_start, y_end;
} check_tile_args;

typedef struct check_tiles_threaded_tasks_
{
    check_tile_args *args_list;
    tile_result *results;
    int n_tasks;
} check_tiles_threaded_tasks;

void * check_tiles_threaded(void *t)
{
    int i;
    /*
    check_tiles_threaded_tasks *tasks = (check_tiles_threaded_tasks *) t;
    for (i = 0; i < tasks->n_tasks; i++) {
        board *b = tasks->args_list[i].b;
        int x_start = tasks->args_list[i].x_start;
        int x_end = tasks->args_list[i].x_end;
        int y_start = tasks->args_list[i].y_start;
        int y_end = tasks->args_list[i].y_end;
        tasks->results[i] = check_tile(*b, x_start, y_start, x_end, y_end);
    }
    */
    pthread_exit(NULL);
}

void check_board_threaded(board *b, int max_density, int tile_width, int n_procs)
{
    int i, rc;
    int n_threads, n_tasks;
    pthread_t *threads;
    check_tile_args *args;
    check_tiles_threaded_tasks *thread_tasks;

    n_tasks = (b->width - tile_width) * (b->width - tile_width);
    n_threads = n_procs - 1;

    threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
    thread_tasks = (check_tiles_threaded_tasks *) malloc(n_threads * sizeof(check_tiles_threaded_tasks));
    args = (check_tile_args *) malloc(n_tasks * sizeof(check_tile_args));

    for (i = 0; i < n_threads; i++) {
        rc = pthread_create(&threads[i], NULL, check_tiles_threaded, (void *) &thread_tasks[i]);
        
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_tasks);
    free(args);
    return;
}