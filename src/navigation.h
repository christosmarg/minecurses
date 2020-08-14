#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "main.h"
#include "minesweeper.h"

extern void navigate(Board *b, int *mv);
extern void getmv(const Board *b, int *mv, int *y, int *x);
extern void mvup(int *y);
extern void mvdown(int *y, int xmax);
extern void mvleft(int *x);
extern void mvright(int *x, int xmax);
extern void curs_update(const Board *b, int y, int x);

#endif /* NAVIGATION_H */
