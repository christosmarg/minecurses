#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <time.h>
#include "wins.h"

#define BLANK ' '
#define MINE  '*'

extern void    displayboard_init(Board *b);
extern void    displayboard_fill(Board *b);
extern void    mineboard_init(Board *b);
extern void    mines_place(Board *b);
extern void    add_adj(Board *b);
extern int     is_mine(const Board *b, int r, int c);
extern int     outof_bounds(const Board *b, int r, int c);
extern uint8_t adj_mines(const Board *b, int r, int c);
extern void    spaces_fill(Board *b);
extern void    die(void);

#endif /* MINESWEEPER_H */
