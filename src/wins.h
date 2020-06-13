#ifndef WINS_H
#define WINS_H

#include "main.h"

#define CENTER(x, y) (x/2 - y/2)

void init_curses (void);
WINDOW *game_win (int rows, int cols);
void options_menu(void);
void fill_menu   (struct _win_st* opts);

#endif /* WINS_H */
