#include <pthread.h>
#include <stdlib.h>
#include <windows.h>

#include "board_threaded.h"
#include "board.h"

#ifdef MAX
#undef MAX
#endif  // MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

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
    check_tiles_threaded_tasks *tasks = (check_tiles_threaded_tasks *) t;
    for (i = 0; i < tasks->n_tasks; i++) {
        board *b = tasks->args_list[i].b;
        int x_start = tasks->args_list[i].x_start;
        int x_end = tasks->args_list[i].x_end;
        int y_start = tasks->args_list[i].y_start;
        int y_end = tasks->args_list[i].y_end;
        tasks->results[i] = check_tile(*b, x_start, y_start, x_end, y_end);
    }
    pthread_exit(NULL);
    return NULL;
}

void check_board_threaded(board *b, int max_density, int tile_width, int n_procs)
{
    int i, j, rc, cur_thread;
    int threshold;
    int n_threads, n_tasks, max_thread_tasks;
    pthread_t *threads;
    check_tiles_threaded_tasks *thread_tasks;

    // Calculate number of tasks, number of threads, and maximum number of tasks per thread.
    n_tasks = (b->width - tile_width) * (b->width - tile_width);
    n_threads = n_procs - 1;
    max_thread_tasks = (n_tasks / n_threads) + 1;

    // Init memory for threads and thread_tasks.
    threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
    thread_tasks = (check_tiles_threaded_tasks *) malloc(n_threads * sizeof(check_tiles_threaded_tasks));

    // Init thread_tasks fields.
    for (i = 0; i < n_threads; i++) {
        thread_tasks[i].args_list = (check_tile_args *) malloc(max_thread_tasks * sizeof(check_tile_args));
        thread_tasks[i].results = (tile_result *) malloc(max_thread_tasks * sizeof(tile_result));
        thread_tasks[i].n_tasks = 0;
    }

    cur_thread = 0;
    // Assign tasks to threads.
    for (i = 0; i < b->width; i += tile_width) {
        for (j = 0; j < b->width; j += tile_width) {
            int cur_task = thread_tasks[cur_thread].n_tasks;
            thread_tasks[cur_thread].args_list[cur_task].b = b;
            thread_tasks[cur_thread].args_list[cur_task].x_start = j;
            thread_tasks[cur_thread].args_list[cur_task].y_start = i;
            thread_tasks[cur_thread].args_list[cur_task].x_end = j + tile_width;
            thread_tasks[cur_thread].args_list[cur_task].y_end = i + tile_width;
            thread_tasks[cur_thread].n_tasks++;
            cur_thread = (cur_thread + 1) % n_threads;
        }
    }

    // Run threads.
    for (i = 0; i < n_threads; i++) {
        rc = pthread_create(&threads[i], NULL, check_tiles_threaded, (void *) &thread_tasks[i]);
    }

    // Wait on each thread unless results surpass threshold.
    threshold = tile_width * tile_width * max_density / 100;
    for (i = 0; i < n_threads && !b->complete; i++) {
        // Block on completion of threads[i].
        pthread_join(threads[i], NULL);

        // Check if any results of thread[i]'s results surpass threshold.
        for (j = 0; j < thread_tasks[i].n_tasks; j++) {
            tile_result result = thread_tasks[i].results[j];
            if (result.red > threshold || result.blue > threshold) {
                b->complete = TRUE;
                b->max_density = (MAX(result.red, result.blue) * 100) / (tile_width * tile_width);
                break;
            }
        }
    }

    // Free memory.
    for (i = 0; i < n_threads; i++) {
        free(thread_tasks[i].args_list);
        free(thread_tasks[i].results);
    }
    free(threads);
    free(thread_tasks);
    return;
}