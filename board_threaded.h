#ifndef RED_BLUE_BOARD_THREADED_H
#define RED_BLUE_BOARD_THREADED_H

#include "board.h"

void check_board_threaded(board *b, int max_density, int tile_width, int n_procs);
void shift_blue_threaded(board *b, int n_procs);
void shift_red_threaded(board *b, int n_procs);

#endif  // RED_BLUE_BOARD_THREADED_H