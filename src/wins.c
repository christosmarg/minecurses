#include "wins.h"

void
init_curses(void)
{
    initscr();
    noecho();
    cbreak();
}

WINDOW *
game_win(int rows, int cols)
{
    int wrows  = GRIDSPACE_Y(rows);
    int wcols  = GRIDSPACE_X(cols);
    int wy     = CENTER(YMAX(stdscr), wrows);
    int wx     = CENTER(XMAX(stdscr), wcols);
    WINDOW *gw = newwin(wrows, wcols, wy, wx);
    wattron(gw, A_BOLD);
    box(gw, 0, 0);
    wrefresh(gw);
    wattroff(gw, A_BOLD);
    return gw;
}

void
options_menu(void)
{
    int w  = 33, h = 15;
    int wy = CENTER(YMAX(stdscr), h);
    int wx = CENTER(XMAX(stdscr), w);
    WINDOW *opts = newwin(h, w, wy, wx);
    werase(opts);
    box(opts, 0, 0);
    fill_menu(opts);
    wrefresh(opts);
    wgetch(opts);
    werase(opts);
    wrefresh(opts);
    delwin(opts);
}

void
fill_menu(WINDOW *opts)
{
    mvwprintw(opts, 1, 1, "q    Quit");
    mvwprintw(opts, 2, 1, "w/k  Move up");
    mvwprintw(opts, 3, 1, "s/j  Move down");
    mvwprintw(opts, 4, 1, "a/h  Move left");
    mvwprintw(opts, 5, 1, "d/l  Move right");
    mvwprintw(opts, 6, 1, "f    Flag cell");
    mvwprintw(opts, 7, 1, "g    Defuse (if flagged only)");
    mvwprintw(opts, 8, 1, "p    Pause music");
    mvwprintw(opts, 9, 1, "+    Volume up");
    mvwprintw(opts, 10, 1, "-   Volume down");
    mvwprintw(opts, 11, 1, "[ENTER]/o Open cell");
    mvwprintw(opts, 13, 1, "Press any key to quit the menu");
}
