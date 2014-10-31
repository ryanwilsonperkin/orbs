#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR(s) {fprintf(stderr, "error: %s\n", s); exit(EXIT_FAILURE);}
int main(int argc, char *argv[])
{
    int n_procs = 0;
    int board_width = 0;
    int tile_width = 0;
    int max_density = 0;
    int max_steps = 0;
    int random_seed = 0;
    bool interactive = false;

    // Get args
    
    if (n_procs <= 0) ERROR("Thread count must be greater than 0.");
    if (board_width <= 1) ERROR("Board width must be greater than 1.");
    if (tile_width <= 0 || board_width % tile_width != 0) ERROR("Tile width must divide board width.");
    if (max_density < 1 || max_density > 100) ERROR("Max density must be a value between 1 and 100.");
    if (max_steps <= 0) ERROR("Max steps must be greater than 0.");
    return 0;
}
