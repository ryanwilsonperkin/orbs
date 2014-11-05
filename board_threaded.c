#include <stdlib.h>

#include "board_threaded.h"
#include "board.h"
#include "pthread.h"

#ifdef MAX
#undef MAX
#endif  // MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

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

void check_board_threaded(board *b, pthread_t *threads, check_tiles_threaded_tasks *thread_tasks, int max_density, int tile_width, int n_procs)
{
    int i, j, rc, cur_thread;
    int threshold;
    int n_threads, n_tasks, max_thread_tasks;

    // Switch to serial version if single processor.
    if (n_procs == 1) {
        check_board(b, max_density, tile_width);
        return;
    }
    // Calculate number of tasks, number of threads, and maximum number of tasks per thread.
    n_tasks = (b->width - tile_width) * (b->width - tile_width);
    n_threads = n_procs - 1;
    max_thread_tasks = (n_tasks / n_threads) + 1;

    // Init thread_tasks fields.
    for (i = 0; i < n_threads; i++) {
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
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
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

    return;
}

void * shift_columns_threaded(void * args_)
{
    int i;
    shift_args *args = (shift_args *) args_;
    for (i = 0; i < args->n_indices; i++) {
        shift_column(args->b, args->indices[i]);
    }
    pthread_exit(NULL);
    return NULL;
}

void shift_blue_threaded(board *b, pthread_t *threads, int n_procs)
{
    int i, j, k, rc;
    int n_tasks, n_threads, max_thread_tasks;
    shift_args *thread_tasks;

    // Switch to serial version if single processor.
    if (n_procs == 1) {
        shift_blue(b);
        return;
    }

    // Calculate number of tasks, number of threads, and maximum number of tasks per thread.
    n_tasks = b->width;
    n_threads = n_procs - 1;
    max_thread_tasks = (n_tasks / n_threads) + 1;

    // Init memory for threads and thread_tasks.
    thread_tasks = (shift_args *) malloc(n_threads * sizeof(shift_args));

    // Init and assign thread_tasks indices.
    for (i = 0; i < n_threads; i++) {
        thread_tasks[i].indices = (int *) malloc(max_thread_tasks * sizeof(int));
        k = 0;
        for (j = i; j < n_tasks; j += n_threads) {
            thread_tasks[i].indices[k] = j;
            k++;
        }
        thread_tasks[i].n_indices = k;
        thread_tasks[i].b = b;

        // Call thread with assigned tasks.
        rc = pthread_create(&threads[i], NULL, shift_columns_threaded, (void *) &thread_tasks[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free memory.
    for (i = 0; i < n_threads; i++) {
        free(thread_tasks[i].indices);
    }
    free(thread_tasks);
}

void * shift_rows_threaded(void * args_)
{
    int i;
    shift_args *args = (shift_args *) args_;
    for (i = 0; i < args->n_indices; i++) {
        shift_row(args->b, args->indices[i]);
    }
    pthread_exit(NULL);
    return NULL;
}

void shift_red_threaded(board *b, pthread_t *threads, int n_procs)
{
    int i, j, k, rc;
    int n_tasks, n_threads, max_thread_tasks;
    shift_args *thread_tasks;

    // Switch to serial version if single processor.
    if (n_procs == 1) {
        shift_red(b);
        return;
    }

    // Calculate number of tasks, number of threads, and maximum number of tasks per thread.
    n_tasks = b->width;
    n_threads = n_procs - 1;
    max_thread_tasks = (n_tasks / n_threads) + 1;

    // Init memory for threads and thread_tasks.
    thread_tasks = (shift_args *) malloc(n_threads * sizeof(shift_args));

    // Init and assign thread_tasks indices.
    for (i = 0; i < n_threads; i++) {
        thread_tasks[i].indices = (int *) malloc(max_thread_tasks * sizeof(int));
        k = 0;
        for (j = i; j < n_tasks; j += n_threads) {
            thread_tasks[i].indices[k] = j;
            k++;
        }
        thread_tasks[i].n_indices = k;
        thread_tasks[i].b = b;

        // Call thread with assigned tasks.
        rc = pthread_create(&threads[i], NULL, shift_rows_threaded, (void *) &thread_tasks[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free memory.
    for (i = 0; i < n_threads; i++) {
        free(thread_tasks[i].indices);
    }
    free(thread_tasks);
}

void shift_board_threaded(board *b, pthread_t *threads, int n_procs)
{
    shift_red_threaded(b, threads, n_procs);
    shift_blue_threaded(b, threads, n_procs);
}