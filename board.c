#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "pthread.h"
#include "wallclock.h"

#ifdef MAX
#undef MAX
#endif  // MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

void init_board(board *b, int board_width, int random_seed)
{
    int i,j;
    srand(random_seed);
    b->points = malloc(board_width * sizeof(int *));
    for (i = 0; i < board_width; i++) {
        b->points[i] = malloc(board_width * sizeof(int));
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
    for (i = 0; i < b.width; i++) {
        for (j = 0; j < b.width; j++) {
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

double check_board(board *b, int max_density, int tile_width, int n_procs)
{
    int i,j;
    int threshold;
    tile_result result;

    StartTime();
    threshold = tile_width * tile_width * max_density / 100;
    for (i = 0; i < b->width; i += tile_width) {
        for (j = 0; j < b->width; j += tile_width) {
            result = check_tile(*b, i, j, i + tile_width, j + tile_width);
            if (result.red > threshold || result.blue > threshold) {
                b->complete = TRUE;
                b->max_density = (MAX(result.red, result.blue) * 100) / (tile_width * tile_width);
                return EndTime();
            }
        }
    }
    return EndTime();
}

double shift_board(board *b, int n_procs)
{
    double red_elapsed_time, blue_elapsed_time;
    red_elapsed_time = shift_red(b, n_procs);
    blue_elapsed_time = shift_blue(b, n_procs);
    return red_elapsed_time + blue_elapsed_time;
}

double shift_red(board *b, int n_procs)
{
    int i,j;
    int neighbor;
    
    StartTime();
    for (i = 0; i < b->width; i++) {
        for (j = 0; j < b->width; j++) {
            neighbor = (j + 1) % b->width;
            if (b->points[i][j] == 1 && b->points[i][neighbor] == 0) {
                b->points[i][j] = 0;
                b->points[i][neighbor] = 1;
                j++;
            }
        }
    }
    return EndTime();
}

double shift_blue(board *b, int n_procs)
{
    int i,j;
    int neighbor;

    StartTime();
    for (j = 0; j < b->width; j++) {
        for (i = 0; i < b->width; i++) {
            neighbor = (i + 1) % b->width;
            if (b->points[i][j] == 2 && b->points[neighbor][j] == 0) {
                b->points[i][j] = 0;
                b->points[neighbor][j] = 2;
                i++;
            }
        }
    }
    return EndTime();
}
