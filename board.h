#ifndef RED_BLUE_BOARD_H
#define RED_BLUE_BOARD_H

#include <stdbool.h>
#include <stdio.h>

typedef struct board_
{
    int **points;
    int width;
    int tile_width;
    bool complete;
    int max_density;
} board;

double check_board(board b, int max_density, int n_procs);
void free_board(board *b);
void init_board(board *b, int board_width, int random_seed);
void print_board(board b, FILE *results_file);
double shift_board(board *b, int n_procs);

#endif  // RED_BLUE_BOARD_H
