#ifndef RED_BLUE_ORBS_H
#define RED_BLUE_ORBS_H

/*
 * orbs
 * Run the red blue simulation on a board up to a maximum number of steps.
 * Prints results to a file called redblue.txt
 *
 * b: The board.
 * n_procs: Number of available processorsx
 * tile_with: Width of tile to use when checking board.
 * max_density: Stopping condition for density of tiles in board.
 * max_steps: Maximum number of shifts to perform on board.
 *
 * returns: Number of shifts performed.
 */
int orbs(board *b, int n_procs, int tile_width, int max_density, int max_steps);

#endif  // RED_BLUE_ORBS_H
