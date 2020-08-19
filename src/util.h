#ifndef UTIL_H 
#define UTIL_H

#include <string.h>
#include "boards.h"
#include "main.h"
#include "wins.h"

#define SESSION_PATH   "log/session.txt"
#define SCORE_LOG_PATH "log/scorelog.csv"

#define YMAX(x)        (getmaxy(x))
#define XMAX(x)        (getmaxx(x))
#define YMID(x)        (getmaxy(x) / 2)
#define XMID(x)        (getmaxx(x) / 2)
#define GRIDSPACE_X(x) (x * 3 + 2)
#define GRIDSPACE_Y(y) (y + 2)
#define ARRSPACE_X(x)  ((x - 2) / 3)
#define ARRSPACE_Y(y)  (y - 1)

typedef enum {
    GAME_WON,
    GAME_LOST
} State;

extern void  util_reset(Minecurses *m);
extern void  util_game_init(Minecurses *m);
extern int   util_cols_set(void);
extern int   util_rows_set(void);
extern int   util_nummines_set(int dimensions);
extern void  util_dealloc_boards(Minecurses *m);
extern void  util_session_write(const Minecurses *m, State state);
extern void  util_score_write(const Minecurses *m);
extern char *util_playername_get(void);
extern void  util_die(void);

#endif /* UTIL_H */
