#ifndef RED_BLUE_RBS_H
#define RED_BLUE_RBS_H

int rbs(board *b, pthread_t *threads, shift_args *shift_thread_tasks, check_tiles_threaded_tasks * check_thread_tasks,
        int n_procs, int tile_width, int max_density, int max_steps);

int rbs_interactive(board *b, pthread_t *threads, shift_args *shift_thread_tasks,
                    check_tiles_threaded_tasks * check_thread_tasks, int n_procs, int tile_width, int max_density,
                    int max_steps);

#endif  // RED_BLUE_RBS_H
