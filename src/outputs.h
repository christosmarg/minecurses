#ifndef OUTPUTS_H
#define OUTPUTS_H

#include "main.h"
#include <string.h>

#define SESSION_PATH "log/session.txt"
#define SCORE_LOG_PATH "log/scorelog.csv"

typedef enum {
	GAME_WON,
	GAME_LOST
} State;

void print_board  (const Board *b);
void print_grid	  (const Board *b);
void session_info (const Board *b);
void session_write(const Board *b, State state);
void score_write  (const Board *b);
char *get_pname	  (void);
void endscreen	  (const Board *b, State state);

#endif /* OUTPUTS_H */
