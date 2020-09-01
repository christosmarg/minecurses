#ifndef BOARDS_H 
#define BOARDS_H

#include <time.h>
#include "main.h"
#include "util.h"

#define BLANK ' '
#define MINE  '*'

#define BOARD_ALLOC(board, rows, cols) \
    size_t i; \
    board = (char **)malloc(rows * sizeof(char *)); \
    for (i = 0; i < rows; i++) \
        board[i] = (char *)malloc(cols); \
    if (board == NULL) util_die();

extern void    dispboard_init(Minecurses *m);
extern void    dispboard_fill(Minecurses *m);
extern void    mineboard_init(Minecurses *m);
extern void    mineboard_mines_place(Minecurses *m);
extern void    mineboard_add_adj(Minecurses *m);
extern int     mineboard_is_mine(const Minecurses *m, int r, int c);
extern uint8_t mineboard_mines_count_adj(const Minecurses *m, int r, int c);
extern int     mineboard_out_of_bounds(const Minecurses *m, int r, int c);
extern void    mineboard_spaces_fill(Minecurses *m);

#endif /* BOARDS_H */
