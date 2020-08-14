#include "minesweeper.h"

void
displayboard_init(Board *b)
{
    int i;
    b->db = (char **)malloc(b->rows * sizeof(char *));
    for (i = 0; i < b->rows; i++)
        b->db[i] = (char *)malloc(b->cols);
    if (b->db == NULL) die();
    else displayboard_fill(b);
}

void
displayboard_fill(Board *b)
{
    int i, j;
    for (i = 0; i < b->rows; i++)
        for (j = 0; j < b->cols; j++)
            b->db[i][j] = BLANK;
}

void
mineboard_init(Board *b)
{
    int i;
    b->mb = (char **)malloc(b->rows * sizeof(char *));
    for (i = 0; i < b->rows; i++)
        b->mb[i] = (char *)malloc(b->cols);

    if (b->mb == NULL) die();
    else
    {
        mines_place(b);
        add_adj(b);
        spaces_fill(b);
    }
}

void
mines_place(Board *b)
{
    int i, r, c;
    srand(time(NULL));
    for (i = 0; i < b->nmines; i++)
    {
        r = rand() % b->rows;
        c = rand() % b->cols;
        b->mb[r][c] = MINE;
    }
}

void
add_adj(Board *b)
{
    int i, j;
    for (i = 0; i < b->rows; i++)
        for (j = 0; j < b->cols; j++)
            if (!is_mine(b, i, j))
                b->mb[i][j] = adj_mines(b, i, j) + '0';                
}

int
is_mine(const Board *b, int r, int c)
{
    return (b->mb[r][c] == MINE) ? TRUE : FALSE;
}

int
outof_bounds(const Board *b, int r, int c)
{
    return (r < 0 || r > b->rows-1 || c < 0 || c > b->cols-1) ? TRUE : FALSE;
}

uint8_t
adj_mines(const Board *b, int r, int c)
{
    uint8_t nadj = 0;

    if (!outof_bounds(b, r, c-1)   && b->mb[r][c-1]   == MINE) nadj++; // North
    if (!outof_bounds(b, r, c+1)   && b->mb[r][c+1]   == MINE) nadj++; // South
    if (!outof_bounds(b, r+1, c)   && b->mb[r+1][c]   == MINE) nadj++; // East
    if (!outof_bounds(b, r-1, c)   && b->mb[r-1][c]   == MINE) nadj++; // West
    if (!outof_bounds(b, r+1, c-1) && b->mb[r+1][c-1] == MINE) nadj++; // North-East
    if (!outof_bounds(b, r-1, c-1) && b->mb[r-1][c-1] == MINE) nadj++; // North-West
    if (!outof_bounds(b, r+1, c+1) && b->mb[r+1][c+1] == MINE) nadj++; // South-East
    if (!outof_bounds(b, r-1, c+1) && b->mb[r-1][c+1] == MINE) nadj++; // South-West

    return nadj;
}

void
spaces_fill(Board *b)
{
    int i, j;
    for (i = 0; i < b->rows; i++)
        for (j = 0; j < b->cols; j++)
            if (b->mb[i][j] != MINE && b->mb[i][j] == '0')
                b->mb[i][j] = '-';
}
