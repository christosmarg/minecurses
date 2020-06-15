#ifndef WINS_H
#define WINS_H

#include "main.h"

#define CENTER(x, y) (x/2 - y/2)

void    curses_init(void);
WINDOW *game_win_init(int rows, int cols);
void    menu_options(void);
void    menu_fill(struct _win_st* opts);

#endif /* WINS_H */
