#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"

void init_board(board *b, int board_width, int random_seed)
{
    srand(random_seed);
    b->points = malloc(board_width * sizeof(int *));
    for (int i = 0; i < board_width; i++) {
        b->points[i] = malloc(board_width * sizeof(int));
        for (int j = 0; j < board_width; j++) {
            b->points[i][j] = rand() % 3;
        }
    }
    b->width = board_width;
    b->complete = false;
    b->max_density = 0;
}

void free_board(board *b)
{
    for (int i = 0; i < b->width; i++) {
        free(b->points[i]);
    }
    free(b->points);
}

void print_board(board b, FILE *results_file)
{
    for (int i = 0; i < b.width; i++) {
        for (int j = 0; j < b.width; j++) {
            switch (b.points[i][j]) {
                case 0:
                    fprintf(results_file, "%c", WHITE_CHAR);
                    break;
                case 1:
                    fprintf(results_file, "%c", RED_CHAR);
                    break;
                case 2:
                    fprintf(results_file, "%c", BLUE_CHAR);
                    break;
            }
        }
        fprintf(results_file,"\n");
    }
}

double check_board(board b, int max_density, int tile_width, int n_procs)
{
    return 0;
}

double shift_board(board *b, int n_procs)
{
    return 0;
}
