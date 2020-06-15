#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <time.h>
#include "wins.h"

#define BLANK ' '
#define MINE  '*'

void    db_init(Board *b);
void    db_fill(Board *b);
void    mb_init(Board *b);
void    mines_place(Board *b);
void    add_adj(Board *b);
int     is_mine(const Board *b, int r, int c);
int     outof_bounds(const Board *b, int r, int c);
uint8_t adj_mines(const Board *b, int r, int c);
void    spaces_fill(Board *b);
void    die(void);

#endif /* MINESWEEPER_H */
