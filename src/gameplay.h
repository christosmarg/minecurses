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

extern void gmpl_start(struct Minecurses *m);

#endif /* GAMEPLAY_H */
