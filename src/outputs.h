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

void print_board  (struct _win_st* gw, Board *b);
void print_grid	  (struct _win_st* gw, Board *b);
void session_info (Board *b);
void session_write(Board *b, State state);
void score_write  (Board *b);
char *get_pname	  (void);
void sort_scorelog(FILE *scorelog);
void show_scorelog(FILE *scorelog);
void parse_data   (FILE *scorelog);
void game_won	  (struct _win_st* gw);
void game_over	  (struct _win_st* gw);

#endif /* OUTPUTS_H */
