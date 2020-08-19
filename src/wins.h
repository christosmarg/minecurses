#ifndef WINS_H
#define WINS_H

#include "util.h"

#define CENTER(x, y) (x/2 - y/2)

extern WINDOW *wins_gamewin_init(int rows, int cols);
extern void    wins_menu_options(void);
extern void    wins_menu_fill(struct _win_st* opts);

#endif /* WINS_H */
