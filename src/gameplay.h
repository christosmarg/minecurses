#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <ctype.h>
#include "audio.h"
#include "util.h"

#define MOVE_ENTER        '\n'
#define MOVE_OPEN_CELL    'o'
#define MOVE_FLAG_CELL    'f'
#define MOVE_DEFUSE_CELL  'g'
#define MOVE_PAUSE_AUDIO  'p'
#define MOVE_VOLUME_UP    '+'
#define MOVE_VOLUME_DOWN  '-'
#define MOVE_OPEN_MENU    'm'
#define MOVE_RESTART      'r'
#define MOVE_QUIT         'q'
#define MINE_DEFUSED      'd'
#define CELL_FLAGGED      'F'

extern void  gmpl_start(Minecurses *m);
extern void  gmpl_board_print(const Minecurses *m);
extern void  gmpl_grid_print(const Minecurses *m);
extern int   gmpl_open_cell(Minecurses *m);
extern void  gmpl_handle_flags(Minecurses *m);
extern void  gmpl_defuse_mine(Minecurses *m);
extern void  gmpl_transfer(Minecurses *m);
extern void  gmpl_reveal(const Minecurses *m);
extern int   gmpl_is_defused(const Minecurses *m);
extern void  gmpl_handle_menu(const Minecurses *m);
extern void  gmpl_handle_gameover(const Minecurses *m);
extern void  gmpl_handle_win(const Minecurses *m);
extern void  gmpl_navigate(Minecurses *m, int *mv);
extern void  gmpl_getmv(const Minecurses *m, int *mv, int *y, int *x);
extern void  gmpl_mvup(int *y);
extern void  gmpl_mvdown(int *y, int xmax);
extern void  gmpl_mvleft(int *x);
extern void  gmpl_mvright(int *x, int xmax);
extern void  gmpl_curs_update(const Minecurses *m, int y, int x);
extern void  gmpl_session_info(const Minecurses *m);
extern void  gmpl_endscreen(const Minecurses *m, State state);

#endif /* GAMEPLAY_H */
