#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"

// Undefine any existing version of MAX macro. Define simple MAX.
#ifdef MAX
#undef MAX
#endif  // MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define CACHELINE 64

void init_board(board *b, int board_width, int random_seed)
{
    int i,j;
    srand(random_seed);
    b->points = (char **) _aligned_malloc(board_width * sizeof(char *), CACHELINE);
    b->columns = (char ***) _aligned_malloc(board_width * sizeof(char **), CACHELINE);
    b->rows = (char ***) _aligned_malloc(board_width * sizeof(char **), CACHELINE);
    for (i = 0; i < board_width; i++) {
        b->points[i] = (char *) _aligned_malloc((board_width + 1) * sizeof(char), CACHELINE);
        b->columns[i] = (char **) _aligned_malloc(board_width * sizeof(char *), CACHELINE);
        b->rows[i] = (char **) _aligned_malloc(board_width * sizeof(char *), CACHELINE);
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
    for (i = 0; i < board_width; i++) {
        for (j = 0; j < board_width; j++) {
            b->columns[j][i] = &b->points[i][j];
            b->rows[i][j] = &b->points[i][j];
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
        _aligned_free(b->points[i]);
        _aligned_free(b->columns[i]);
        _aligned_free(b->rows[i]);
    }
    _aligned_free(b->points);
    _aligned_free(b->columns);
    _aligned_free(b->rows);
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
    int i,j,k,terminator;
    int threshold, board_max_density;
    int tile_max, thread_max, board_max;
    tile_result result;

    tile_max = 0;
    board_max = 0;
    terminator = b->width * b->width / tile_width;
    #pragma omp parallel private(thread_max) shared(board_max)
    {
        thread_max = 0;
        #pragma omp for private(i,j,k,result,tile_max)
        for (k = 0; k < terminator; k += tile_width) {
            i = (k / b->width) * tile_width;
            j = k % b->width;
            result = check_tile(*b, j, i, j + tile_width, i + tile_width);
            tile_max = MAX(result.red, result.blue);
            thread_max = MAX(tile_max, thread_max);
        }
        if (thread_max > board_max) {
            #pragma omp critical (BOARDMAX)
            board_max = MAX(thread_max, board_max);
        }
    }

    threshold = tile_width * tile_width * max_density / 100;
    board_max_density = (board_max * 100) / (tile_width * tile_width);
    if (board_max > threshold) {
        b->complete = TRUE;
        b->max_density = board_max_density;
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
#pragma omp parallel for private(i)
    for (i = 0; i < b->width; i++) {
        shift_row(b->rows[i], b->width);
    }
}

void shift_row(char **row, int length)
{
    int j;
    char init_first, init_last;
    init_first = *row[0];
    init_last = *row[length - 1];
    
    for (j = 0; j < (length - 1); j++) {
        if (*row[j] == RED && *row[j+1] == WHITE) {
            *row[j] = WHITE;
            *row[j+1] = RED;
            j++;
        }
    }

    if ((*row[length - 1] == RED && init_last == RED) &&
            (*row[0] == WHITE && init_first == WHITE)) {
        *row[length - 1] = WHITE;
        *row[0] = RED;
    }
}

void shift_blue(board *b)
{
    int j;
#pragma omp parallel for private(j)
    for (j = 0; j < b->width; j++) {
        shift_column(b->columns[j], b->width);
    }
}

void shift_column(char **column, int length)
{
    int i;
    char init_first, init_last;
    init_first = *column[0];
    init_last = *column[length - 1];

    for (i = 0; i < (length - 1); i++) {
        if (*column[i] == BLUE && *column[i+1] == WHITE) {
            *column[i] = WHITE;
            *column[i+1] = BLUE;
            i++;
        }
    }

    if ((*column[length - 1] == BLUE && init_last == BLUE) &&
            (*column[0] == WHITE && init_first == WHITE)) {
        *column[length - 1] = WHITE;
        *column[0] = BLUE;
    }
}
