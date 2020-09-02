#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <ncurses.h>

struct Minecurses {
    char  **dispboard;
    char  **mineboard;
    int     rows, cols;
    int     nummines, numdefused;
    int     gameover;
    int     x, y;
    WINDOW *gamewin;
};

extern struct Minecurses m;

#endif /* MAIN_H */
