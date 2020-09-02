#ifndef UTIL_H 
#define UTIL_H

#include <string.h>
#include "boards.h"
#include "main.h"
#include "wins.h"

#define SESSION_PATH   "log/session.txt"
#define SCORE_LOG_PATH "log/scorelog.csv"

#define YMAX(x)        (getmaxy((x)))
#define XMAX(x)        (getmaxx((x)))
#define YMID(x)        (getmaxy((x)) / 2)
#define XMID(x)        (getmaxx((x)) / 2)
#define GRIDSPACE_X(x) ((x) * 3 + 2)
#define GRIDSPACE_Y(y) ((y) + 2)
#define ARRSPACE_X(x)  (((x) - 2) / 3)
#define ARRSPACE_Y(y)  ((y) - 1)

typedef enum {
    GAME_WON,
    GAME_LOST
} State;

extern void  util_reset(struct Minecurses *);
extern void  util_game_init(struct Minecurses *);
extern void  util_dealloc_boards(struct Minecurses *);
extern void  util_session_write(const struct Minecurses *, State);
extern void  util_score_write(const struct Minecurses *);
extern void  util_die(void);

#endif /* UTIL_H */
