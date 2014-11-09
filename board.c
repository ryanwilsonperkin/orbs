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
    b->points = (char **) malloc(board_width * sizeof(char *));
    for (i = 0; i < board_width; i++) {
        b->points[i] = (char *) malloc((board_width + 1) * sizeof(char));
        for (j = 0; j < board_width; j++) {
            switch(rand() % 3) {
                case 0:
                    b->points[i][j] = WHITE;
                    break;
                case 1:
                    b->points[i][j] = RED;
                    break;
                case 2:
                    b->points[i][j] = BLUE;
                    break;
            }
        }
        b->points[i][board_width] = '\0';
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
    int i;
    for (i = 0; i < b.width; i++) {
        fprintf(results_file, "%s\n", b.points[i]);
    }
}

tile_result check_tile(board b, int x_start, int y_start, int x_end, int y_end)
{
    int i,j;
    tile_result result = {0, 0, 0};
    for (i = y_start; i < y_end; i++) {
        for (j = x_start; j < x_end; j++) {
            switch (b.points[i][j]) {
                case WHITE:
                    result.white++;
                    break;
                case RED:
                    result.red++;
                    break;
                case BLUE:
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
    int j;
    char init_first, init_last;
    init_first = b->points[index][0];
    init_last = b->points[index][b->width - 1];
    
    for (j = 0; j < (b->width - 1); j++) {
        if (b->points[index][j] == RED && b->points[index][j+1] == WHITE) {
            b->points[index][j] = WHITE;
            b->points[index][j+1] = RED;
            j++;
        }
    }

    if ((b->points[index][b->width - 1] == RED && init_last == RED) &&
            (b->points[index][0] == WHITE && init_first == WHITE)) {
        b->points[index][b->width - 1] = WHITE;
        b->points[index][0] = RED;
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
    int i;
    char init_first, init_last;
    init_first = b->points[0][index];
    init_last = b->points[b->width - 1][index];

    for (i = 0; i < (b->width - 1); i++) {
        if (b->points[i][index] == BLUE && b->points[i+1][index] == WHITE) {
            b->points[i][index] = WHITE;
            b->points[i+1][index] = BLUE;
            i++;
        }
    }

    if ((b->points[b->width - 1][index] == BLUE && init_last == BLUE) && 
            (b->points[0][index] == WHITE && init_first == WHITE)) {
        b->points[b->width - 1][index] = WHITE;
        b->points[0][index] = BLUE;
    }
}
