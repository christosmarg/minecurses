#ifndef MAIN_H
#define MAIN_H

#if defined linux || defined __unix__ || defined __APPLE__
#include <ncurses.h>
#endif /* defined linux || defined __unix__ || defined __APPLE__ */
#include <stdlib.h>

#define YMAX(x)        (getmaxy(x))
#define XMAX(x)        (getmaxx(x))
#define GRIDSPACE_X(x) (x * 3 + 2)
#define GRIDSPACE_Y(y) (y + 2)
#define ARRSPACE_X(x)  ((x - 2) / 3)
#define ARRSPACE_Y(y)  (y - 1)

typedef struct {
    char **db;
    char **mb;
    int rows, cols;
    int nmines, ndefused;
    int gameover;
    int x, y;
    WINDOW *gw;
} Board;

extern Board b;

extern void reset(Board *b);
extern void game_init(Board *b);
extern void dealloc_board(Board *b);
extern void die(void);

#endif /* MAIN_H */
