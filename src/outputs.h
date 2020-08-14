#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <string.h>
#include "main.h"

#define SESSION_PATH   "log/session.txt"
#define SCORE_LOG_PATH "log/scorelog.csv"

#define YMID(x) (getmaxy(x) / 2)
#define XMID(x) (getmaxx(x) / 2)

typedef enum {
    GAME_WON,
    GAME_LOST
} State;

extern void  board_print(const Board *b);
extern void  grid_print(const Board *b);
extern void  session_info(const Board *b);
extern void  session_write(const Board *b, State state);
extern void  score_write(const Board *b);
extern char *playername_get(void);
extern void  endscreen(const Board *b, State state);

#endif /* OUTPUTS_H */
