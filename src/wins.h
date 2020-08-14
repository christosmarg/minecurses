#ifndef WINS_H
#define WINS_H

#include "main.h"

#define CENTER(x, y) (x/2 - y/2)

extern void    curses_init(void);
extern WINDOW *game_win_init(int rows, int cols);
extern void    menu_options(void);
extern void    menu_fill(struct _win_st* opts);

#endif /* WINS_H */
