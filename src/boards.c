#include "boards.h"

void
dispboard_init(Minecurses *m)
{
    int i;
    m->dispboard = (char **)malloc(m->rows * sizeof(char *));
    for (i = 0; i < m->rows; i++)
        m->dispboard[i] = (char *)malloc(m->cols);
    if (m->dispboard == NULL) util_die();
    else dispboard_fill(m);
}

void
dispboard_fill(Minecurses *m)
{
    int i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            m->dispboard[i][j] = BLANK;
}

void
mineboard_init(Minecurses *m)
{
    int i;
    m->mineboard = (char **)malloc(m->rows * sizeof(char *));
    for (i = 0; i < m->rows; i++)
        m->mineboard[i] = (char *)malloc(m->cols);

    if (m->mineboard == NULL) util_die();
    else
    {
        mineboard_mines_place(m);
        mineboard_add_adj(m);
        mineboard_spaces_fill(m);
    }
}

void
mineboard_mines_place(Minecurses *m)
{
    int i, r, c;
    srand(time(NULL));
    for (i = 0; i < m->nummines; i++)
    {
        r = rand() % m->rows;
        c = rand() % m->cols;
        m->mineboard[r][c] = MINE;
    }
}

void
mineboard_add_adj(Minecurses *m)
{
    int i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (!mineboard_is_mine(m, i, j))
                m->mineboard[i][j] = mineboard_mines_count_adj(m, i, j) + '0';
}

int
mineboard_is_mine(const Minecurses *m, int r, int c)
{
    return (m->mineboard[r][c] == MINE) ? TRUE : FALSE;
}

uint8_t
mineboard_mines_count_adj(const Minecurses *m, int r, int c)
{
    uint8_t nadj = 0;

    if (!mineboard_out_of_bounds(m, r, c-1)   && m->mineboard[r][c-1]   == MINE) nadj++; // North
    if (!mineboard_out_of_bounds(m, r, c+1)   && m->mineboard[r][c+1]   == MINE) nadj++; // South
    if (!mineboard_out_of_bounds(m, r+1, c)   && m->mineboard[r+1][c]   == MINE) nadj++; // East
    if (!mineboard_out_of_bounds(m, r-1, c)   && m->mineboard[r-1][c]   == MINE) nadj++; // West
    if (!mineboard_out_of_bounds(m, r+1, c-1) && m->mineboard[r+1][c-1] == MINE) nadj++; // North-East
    if (!mineboard_out_of_bounds(m, r-1, c-1) && m->mineboard[r-1][c-1] == MINE) nadj++; // North-West
    if (!mineboard_out_of_bounds(m, r+1, c+1) && m->mineboard[r+1][c+1] == MINE) nadj++; // South-East
    if (!mineboard_out_of_bounds(m, r-1, c+1) && m->mineboard[r-1][c+1] == MINE) nadj++; // South-West

    return nadj;
}

int
mineboard_out_of_bounds(const Minecurses *m, int r, int c)
{
    return (r < 0 || r > m->rows-1 || c < 0 || c > m->cols-1) ? TRUE : FALSE;
}

void
mineboard_spaces_fill(Minecurses *m)
{
    int i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (m->mineboard[i][j] != MINE && m->mineboard[i][j] == '0')
                m->mineboard[i][j] = '-';
}
