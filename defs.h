#ifndef DEFS_H
#define DEFS_H

/* constants */
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
#define BLANK             ' '
#define MINE              '*'
#define VOL_STEP          10
#define SESSION_PATH      "log/session.txt"
#define SCORE_LOG_PATH    "log/scorelog.csv"
#define AUDIO_PATH        "res/detective-8bit.wav"

/* macros */
#define BOARD_ALLOC(board, rows, cols)  do {            \
    size_t i;                                           \
    (board) = (char **)malloc((rows) * sizeof(char *)); \
    for (i = 0; i < (rows); i++)                        \
        (board)[i] = (char *)malloc((cols));            \
    if ((board) == NULL) die();                         \
} while (0)

#define YMAX(x)        (getmaxy((x)))
#define XMAX(x)        (getmaxx((x)))
#define YMID(x)        (getmaxy((x)) / 2)
#define XMID(x)        (getmaxx((x)) / 2)
#define GRIDSPACE_X(x) ((x) * 3 + 2)
#define GRIDSPACE_Y(y) ((y) + 2)
#define ARRSPACE_X(x)  (((x) - 2) / 3)
#define ARRSPACE_Y(y)  ((y) - 1)
#define CENTER(x, y)   ((x) / 2 - (y) / 2)

#endif /* DEFS_H */
