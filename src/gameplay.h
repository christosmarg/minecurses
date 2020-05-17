#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "audio.h"
#include "main.h"
#include "navigation.h"
#include "outputs.h"
#include "wins.h"

#define BLANK ' '
#define MINE '*'
#define ENTER '\n'
#define OPEN_LOWER 'o'
#define OPEN_UPPER 'O'
#define FLAG 'F'
#define FLAG_LOWER 'f'
#define FLAG_UPPER 'F'
#define DEFUSE_LOWER 'g'
#define DEFUSE_UPPER 'G'
#define DEFUSED 'D'
#define PAUSE_AUDIO 'p'
#define VOLUME_UP '+'
#define VOLUME_DOWN '-'
#define QUIT 'q'

void play			(struct _win_st* gw, Board *b);
int open_cell		(struct _win_st* gw, Board *b);
void handle_flags	(struct _win_st* gw, Board *b);
void defuse_mine	(struct _win_st* gw, Board *b);
void transfer		(Board *b);
void reveal			(struct _win_st* gw, Board *b);
int is_defused		(Board *b);
void handle_menu	(struct _win_st*, Board *b);
void handle_gameover(struct _win_st* gw, Board *b);
void handle_win		(struct _win_st* gw, Board *b);

#endif /* GAMEPLAY_H */
