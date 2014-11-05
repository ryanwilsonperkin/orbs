#ifndef RED_BLUE_BOARD_THREADED_H
#define RED_BLUE_BOARD_THREADED_H

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

typedef struct shift_args_
{
    board *b;
    int *indices;
    int n_indices;
} shift_args;

void check_board_threaded(board *b, int max_density, int tile_width, int n_procs);
void shift_board_threaded(board *b, int n_procs);
void shift_blue_threaded(board *b, int n_procs);
void shift_red_threaded(board *b, int n_procs);

#endif  // RED_BLUE_BOARD_THREADED_H