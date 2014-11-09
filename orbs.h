#ifndef RED_BLUE_ORBS_H
#define RED_BLUE_ORBS_H

/*
 * orbs
 * Run the red blue simulation on a board up to a maximum number of steps.
 * If threads and tasks are provided, parallelize the operation.
 * Prints results to a file called redblue.txt
 *
 * b: The board.
 * threads: Threads to use for parallel execution of tasks. NULL if serial.
 * shift_thread_tasks: Task layout for shifting board. NULL if serial.
 * check_tiles_threaded_tasks: Task layout for checking board. NULL if serial.
 * n_procs: Number of available processorsx
 * tile_with: Width of tile to use when checking board.
 * max_density: Stopping condition for density of tiles in board.
 * max_steps: Maximum number of shifts to perform on board.
 *
 * returns: Number of shifts performed.
 */
int orbs(board *b, pthread_t *threads, shift_args *shift_thread_tasks, check_tiles_threaded_tasks * check_thread_tasks,
        int n_procs, int tile_width, int max_density, int max_steps);

/*
 * orbs_interactive
 * Interactively run the red blue simulation on a board up to a maximum number of steps.
 * If threads and tasks are provided, parallelize the operation.
 * Prints results to a file called redblue.txt
 *
 * Press enter to perform one full step.
 * Press #n to perform n full steps (where n is an integer).
 * Press h to pergorm one half step (either red or blue shift).
 * Press c to continue in automatic mode to normal termination and output file.
 * Press x to output current board and exit.
 *
 * b: The board.
 * threads: Threads to use for parallel execution of tasks. NULL if serial.
 * shift_thread_tasks: Task layout for shifting board. NULL if serial.
 * check_tiles_threaded_tasks: Task layout for checking board. NULL if serial.
 * n_procs: Number of available processorsx
 * tile_with: Width of tile to use when checking board.
 * max_density: Stopping condition for density of tiles in board.
 * max_steps: Maximum number of shifts to perform on board.
 *
 * returns: Number of shifts performed.
 */
int orbs_interactive(board *b, pthread_t *threads, shift_args *shift_thread_tasks,
                    check_tiles_threaded_tasks * check_thread_tasks, int n_procs, int tile_width, int max_density,
                    int max_steps);

#endif  // RED_BLUE_ORBS_H
