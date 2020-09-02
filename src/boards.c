#include "boards.h"

static void    dispboard_fill(struct Minecurses *);
static void    mineboard_mines_place(struct Minecurses *);
static void    mineboard_add_adj(struct Minecurses *);
static int     mineboard_is_mine(const struct Minecurses *, int, int);
static uint8_t mineboard_mines_count_adj(const struct Minecurses *, int, int);
static int     mineboard_out_of_bounds(const struct Minecurses *, int, int);
static void    mineboard_spaces_fill(struct Minecurses *);

void
dispboard_init(struct Minecurses *m)
{
    BOARD_ALLOC(m->dispboard, m->rows, m->cols);
    dispboard_fill(m);
}

void
dispboard_fill(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            m->dispboard[i][j] = BLANK;
}

void
mineboard_init(struct Minecurses *m)
{
    BOARD_ALLOC(m->mineboard, m->rows, m->cols);
    mineboard_mines_place(m);
    mineboard_add_adj(m);
    mineboard_spaces_fill(m);
}

void
mineboard_mines_place(struct Minecurses *m)
{
    size_t i, r, c;
    srand(time(NULL));
    for (i = 0; i < m->nummines; i++)
    {
        r = rand() % m->rows;
        c = rand() % m->cols;
        m->mineboard[r][c] = MINE;
    }
}

void
mineboard_add_adj(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (!mineboard_is_mine(m, i, j))
                m->mineboard[i][j] = mineboard_mines_count_adj(m, i, j) + '0';
}

int
mineboard_is_mine(const struct Minecurses *m, int r, int c)
{
    return (m->mineboard[r][c] == MINE);
}

uint8_t
mineboard_mines_count_adj(const struct Minecurses *m, int r, int c)
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
mineboard_out_of_bounds(const struct Minecurses *m, int r, int c)
{
    return (r < 0 || r > m->rows-1 || c < 0 || c > m->cols-1);
}

void
mineboard_spaces_fill(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (m->mineboard[i][j] != MINE && m->mineboard[i][j] == '0')
                m->mineboard[i][j] = '-';
}
