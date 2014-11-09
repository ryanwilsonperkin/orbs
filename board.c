#include <stdio.h>
#include <stdlib.h>

#include "board.h"

// Undefine any existing version of MAX macro. Define simple MAX.
#ifdef MAX
#undef MAX
#endif  // MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

void init_board(board *b, int board_width, int random_seed)
{
    int i,j;
    srand(random_seed);
    b->points = (int **) malloc(board_width * sizeof(int *));
    for (i = 0; i < board_width; i++) {
        b->points[i] = (int *) malloc(board_width * sizeof(int));
        for (j = 0; j < board_width; j++) {
            b->points[i][j] = rand() % 3;
        }
    }
    b->width = board_width;
    b->complete = FALSE;
    b->max_density = 0;
}

void free_board(board *b)
{
    int i;
    for (i = 0; i < b->width; i++) {
        free(b->points[i]);
    }
    free(b->points);
}

void print_board(board b, FILE *results_file)
{
    int i,j;
    char c;
    for (i = 0; i < b.width; i++) {
        for (j = 0; j < b.width; j++) {
            switch (b.points[i][j]) {
                case 0:
                    c = WHITE_CHAR;
                    break;
                case 1:
                    c = RED_CHAR;
                    break;
                case 2:
                    c = BLUE_CHAR;
                    break;
            }
            fprintf(results_file, "%c", c);
        }
        fprintf(results_file,"\n");
    }
}

tile_result check_tile(board b, int x_start, int y_start, int x_end, int y_end)
{
    int i,j;
    tile_result result = {0, 0, 0};
    for (i = y_start; i < y_end; i++) {
        for (j = x_start; j < x_end; j++) {
            switch (b.points[i][j]) {
                case 0:
                    result.white++;
                    break;
                case 1:
                    result.red++;
                    break;
                case 2:
                    result.blue++;
                    break;
            }
        }
    }
    return result;
}

void check_board(board *b, int max_density, int tile_width)
{
    int i,j;
    int threshold;
    tile_result result;

    threshold = tile_width * tile_width * max_density / 100;
    for (i = 0; i < b->width; i += tile_width) {
        for (j = 0; j < b->width; j += tile_width) {
            result = check_tile(*b, j, i, j + tile_width, i + tile_width);
            if (result.red > threshold || result.blue > threshold) {
                b->complete = TRUE;
                b->max_density = (MAX(result.red, result.blue) * 100) / (tile_width * tile_width);
                return;
            }
        }
    }
}

void shift_board(board *b)
{
    shift_red(b);
    shift_blue(b);
}

void shift_red(board *b)
{
    int i;
    for (i = 0; i < b->width; i++) {
        shift_row(b, i);
    }
}

void shift_row(board *b, int index)
{
    int j, init_first, init_last;
    init_first = b->points[index][0];
    init_last = b->points[index][b->width - 1];
    
    for (j = 0; j < (b->width - 1); j++) {
        if (b->points[index][j] == 1 && b->points[index][j+1] == 0) {
            b->points[index][j] = 0;
            b->points[index][j+1] = 1;
            j++;
        }
    }

    if ((b->points[index][b->width - 1] == 1 && init_last == 1) &&
            (b->points[index][0] == 0 && init_first == 0)) {
        b->points[index][b->width - 1] = 0;
        b->points[index][0] = 1;
    }
}

void shift_blue(board *b)
{
    int j;
    for (j = 0; j < b->width; j++) {
        shift_column(b, j);
    }
}

void shift_column(board *b, int index)
{
    int i, init_first, init_last;
    init_first = b->points[0][index];
    init_last = b->points[b->width - 1][index];

    for (i = 0; i < (b->width - 1); i++) {
        if (b->points[i][index] == 2 && b->points[i+1][index] == 0) {
            b->points[i][index] = 0;
            b->points[i+1][index] = 2;
            i++;
        }
    }

    if ((b->points[b->width - 1][index] == 2 && init_last == 2) && 
            (b->points[0][index] == 0 && init_first == 0)) {
        b->points[b->width - 1][index] = 0;
        b->points[0][index] = 2;
    }
}
