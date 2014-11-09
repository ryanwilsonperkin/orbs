#ifndef OPENMP_RED_BLUE_BOARD_H
#define OPENMP_RED_BLUE_BOARD_H

#include <stdio.h>

#define TRUE 1
#define FALSE 0

#define WHITE ' '
#define RED '>'
#define BLUE 'V'

/*
 * board
 * Internal representation of a board used for red blue algorithm.
 *
 * points: A two-dimensional array of white, red, and blue points.
 * width: The width of the board.
 * complete: A flag to reflect whether the board has encountered maximum density on a tile. 1 for true. 0 for false.
 * max_density: Density of most dense tile on board.
 */
typedef struct board_
{
    char **points;
    int width;
    char complete;
    int max_density;
} board;

/*
 * tile_result
 * Count of how many white, red, and blue points appear in a tile of board.
 *
 * white: Number of white points.
 * red: Number of red points.
 * blue: Number of blue points.
 */
typedef struct tile_result_
{
    int white;
    int red;
    int blue;
} tile_result;

/*
 * check_tile
 * Count the number of white, red, and blue points in a given tile of the board.
 * Start and end row and column mark the 4 points of the tile.
 *
 * b: The board.
 * x_start: The starting column.
 * y_start: The starting row.
 * x_end: The ending column.
 * y_end: The ending column.
 *
 * Returns tile_result of the point densities.
 */
tile_result check_tile(board b, int x_start, int y_start, int x_end, int y_end);

/*
 * check_board
 * Checks if a board has met the terminating conditions of red blue algorithm.
 *
 * b: The board.
 * max_density: The terminating density of the algorithm.
 * tile_width: Width of tiles to check within board.
 */
void check_board(board *b, int max_density, int tile_width);

/*
 * free_board
 * Free dynamically allocated memory in board.
 *
 * b: The board.
 */
void free_board(board *b);

/*
 * init_board
 * Initialize the board.
 *
 * board_width: Width of the board.
 * random_seed: The seed to pass to srand before generating board tiles.
 */
void init_board(board *b, int board_width, int random_seed);

/*
 * print_board
 * Print board to file or stdout.
 *
 * b: The board.
 * results_file: File to print results to.
 * colour_mod: Flag to enable colour printing. 1 for true. 0 for false. (stdout only).
 */
void print_board(board b, FILE *results_file);

/*
 * shift_board
 * Serially shift colours in board.
 *
 * b: The board.
 */
void shift_board(board *b);

/*
 * shift_red
 * Serially shift all the red colours in the board.
 *
 * b: The board.
 */
void shift_red(board *b);

/*
 * shift_blue
 * Serially shift all the blue colours in the board.
 *
 * b: The board.
 */
void shift_blue(board *b);

/*
 * shift_row
 * Serially shift all the red colours in a single row of the board.
 *
 * b: The board.
 * index: The row index.
 */
void shift_row(board *b, int index);

/*
 * shift_column
 * Serially shift all the blue colours in a single column of the board.
 *
 * b: The board.
 * index: The column index.
 */
void shift_column(board *b, int index);

#endif  // OPENMP_RED_BLUE_BOARD_H
