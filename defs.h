/* See LICENSE file for copyright and license details. */
#ifndef DEFS_H
#define DEFS_H

#define MOVE_ENTER              '\n'
#define MOVE_OPEN_CELL          'o'
#define MOVE_FLAG_CELL          'f'
#define MOVE_DEFUSE_CELL        'g'
#define MOVE_PAUSE_AUDIO        'p'
#define MOVE_VOLUME_UP          '+'
#define MOVE_VOLUME_DOWN        '-'
#define MOVE_OPEN_MENU          'm'
#define MOVE_RESTART            'r'
#define MOVE_QUIT               'q'
#define MINE_DEFUSED            'd'
#define CELL_FLAGGED            'F'
#define CELL_BLANK              ' '
#define CELL_MINE               '*'
#define VOL_STEP                10
#define GRID_BOX                "[ ]"

#define OPT_QUIT                "q      Quit"
#define OPT_MOVE_UP             "w/k    Move up"
#define OPT_MOVE_DOWN           "s/j    Move down"
#define OPT_MOVE_LEFT           "a/h    Move left"
#define OPT_MOVE_RIGHT          "d/l    Move right"
#define OPT_FLAG_CELL           "f      Flag cell"
#define OPT_DEFUSE              "g      Defuse (if flagged only)"
#define OPT_PAUSE_MUS           "p      Pause music"
#define OPT_VOL_UP              "+      Volume up"
#define OPT_VOL_DOWN            "-      Volume down"
#define OPT_OPEN_CELL           "[ENTER]/o Open cell"

#define MSG_COLS                "Columns (Min = %d, Max = %d): "
#define MSG_ROWS                "Rows (Min = %d, Max = %d): "
#define MSG_MINES               "Mines (Min = %d, Max = %d): "
#define MSG_QUIT_MENU           "Press any key to quit the menu"
#define MSG_CONT                "Press any key to continue"
#define MSG_NEW_SCORE           "New score written to score log"
#define MSG_WIN1                "You defused all the mines!"
#define MSG_WIN2                "You won :)"
#define MSG_LOSE1               "You hit a mine! (or tried to defuse the wrong cell)"
#define MSG_LOSE2               "Game over :("

#define GAME_LOST               0
#define GAME_WON                1

#define CURSES_INIT() do {      \
        initscr();              \
        noecho();               \
        cbreak();               \
} while (0)

#define MINEBOARD_INIT(m) do {  \
        minesplace(m);          \
        adjadd(m);              \
        spacesfill(m);          \
} while (0)

#define GAME_RESTART(m) do {    \
        boardsdealloc(m);       \
        gamereset(m);           \
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
#define CURS_UPDATE(m, y, x)   (wmove((m)->gamewin, (y), (x)))
#define IS_MINE(m, r, c)       ((m)->mineboard[(r)][(c)] == CELL_MINE)
#define OUT_OF_BOUNDS(m, r, c) ((r) < 0 || (r) > ((m)->rows - 1) || \
                                (c) < 0 || (c) > ((m)->cols - 1))

#endif /* DEFS_H */
