#ifndef DEFS_H
#define DEFS_H

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
#define CELL_BLANK        ' '
#define CELL_MINE         '*'
#define VOL_STEP          10
#define SESSION_PATH      "log/session.txt"
#define SCORE_LOG_PATH    "log/scorelog.csv"
#define AUDIO_PATH        "res/detective-8bit.wav"

#define CURSES_INIT()                                                  \
    initscr();                                                         \
    noecho();                                                          \
    cbreak();

#define AUDIO_PAUSE() do {                                             \
    (Mix_PausedMusic() == 1) ? Mix_ResumeMusic() : Mix_PauseMusic();   \
} while (0)

#define YMAX(x)                (getmaxy((x)))
#define XMAX(x)                (getmaxx((x)))
#define YMID(x)                (getmaxy((x)) / 2)
#define XMID(x)                (getmaxx((x)) / 2)
#define GRIDSPACE_X(x)         ((x) * 3 + 2)
#define GRIDSPACE_Y(y)         ((y) + 2)
#define ARRSPACE_X(x)          (((x) - 2) / 3)
#define ARRSPACE_Y(y)          ((y) - 1)
#define CENTER(x, y)           ((x) / 2 - (y) / 2)
#define CURS_UPDATE(m, y, x)   (wmove(m->gamewin, (y), (x)))
#define IS_MINE(m, r, c)       ((m->mineboard[r][c]) == CELL_MINE)
#define OUT_OF_BOUNDS(m, r, c) ((r) < 0 || (r) > ((m->rows)-1) ||      \
                                (c) < 0 || (c) > ((m->cols)-1))

#endif /* DEFS_H */
