#include <ctype.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "defs.h"

#ifdef _WIN_32
typedef unsigned char uint8_t
#endif /* _WIN_32 */

/* structs and enums */
struct Minecurses {
    char  **dispboard;
    char  **mineboard;
    int     rows, cols;
    int     nummines, numdefused;
    int     gameover;
    int     x, y;
    WINDOW *gamewin;
};

enum State {
    GAME_WON,
    GAME_LOST
};

/* function declarations */
static void    game_init(struct Minecurses *);
static void    game_reset(struct Minecurses *);
static void    game_start(struct Minecurses *);
static int     cols_set(void);
static int     rows_set(void);
static int     nummines_set(int);
static void    dispboard_init(struct Minecurses *);
static void    dispboard_fill(struct Minecurses *);
static void    mineboard_init(struct Minecurses *);
static void    mineboard_mines_place(struct Minecurses *);
static void    mineboard_add_adj(struct Minecurses *);
static uint8_t mineboard_mines_count_adj(const struct Minecurses *, int, int);
static void    mineboard_spaces_fill(struct Minecurses *);
static void    board_navigate(struct Minecurses *, int *);
static void    boards_dealloc(struct Minecurses *);
static int     cell_open(struct Minecurses *);
static void    cell_transfer(struct Minecurses *);
static void    cell_reveal(const struct Minecurses *);
static void    flags_handle(struct Minecurses *);
static void    mine_defuse(struct Minecurses *);
static void    mvget(const struct Minecurses *, int *, int *, int *);
static void    mvup(int *);
static void    mvdown(int *, int);
static void    mvleft(int *);
static void    mvright(int *, int);
static void    curses_init(void);
static void    board_print(const struct Minecurses *);
static void    grid_print(const struct Minecurses *);
static WINDOW *gamewin_init(int, int);
static void    menu_handle(const struct Minecurses *);
static void    menu_options(void);
static void    menu_fill(struct _win_st *);
static void    session_info(const struct Minecurses *);
static void    win_handle(const struct Minecurses *);
static void    gameover_handle(const struct Minecurses *);
static void    endscreen(const struct Minecurses *, const enum State);
static void    session_write(const struct Minecurses *, const enum State);
static void    score_write(const struct Minecurses *);
static char   *playername_get(void);
static void   *audio_play(void *);
static void    audio_change_volume(char);
static void    die(void);

/* function implementations */
void
game_init(struct Minecurses *m)
{
    dispboard_init(m);
    mineboard_init(m);
}

void
game_reset(struct Minecurses *m)
{
    echo();
    m->cols       = cols_set();
    m->rows       = rows_set();
    m->nummines   = nummines_set(m->rows * m->cols);
    m->numdefused = m->x = m->y = 0;
    m->gameover   = FALSE;
    noecho();
    menu_options();
    erase();
    refresh();
}

#define bx m->x
#define by m->y

void
game_start(struct Minecurses *m)
{
    m->x = m->y = 0;
    m->gameover = FALSE;
    m->numdefused = 0;
    int move;

    do {
        erase();
        delwin(m->gamewin);
        refresh();
        m->gamewin = gamewin_init(m->rows, m->cols);
        board_print(m);
        session_info(m);
        board_navigate(m, &move);

        switch (tolower((char)move)) {
            case MOVE_ENTER:         /* FALLTHROUGH */
            case MOVE_OPEN_CELL:
                m->gameover = cell_open(m);
                break;
            case MOVE_FLAG_CELL:
                flags_handle(m);
                break;
            case MOVE_DEFUSE_CELL:
                if (m->dispboard[by][bx] == CELL_FLAGGED
                && m->mineboard[by][bx] == CELL_MINE)
                {
                    m->numdefused++;
                    mine_defuse(m);
                }
                else if (m->dispboard[by][bx] == CELL_FLAGGED
                      && m->mineboard[by][bx] != CELL_MINE)
                    m->gameover = TRUE;              
                break;
            case MOVE_PAUSE_AUDIO:
                AUDIO_PAUSE();
                break;
            case MOVE_VOLUME_UP:     /* FALLTHROUGH */
            case MOVE_VOLUME_DOWN:
                audio_change_volume(move);
                break;
            case MOVE_OPEN_MENU:
                menu_handle(m);
                break;
            case MOVE_RESTART:
                boards_dealloc(m);
                game_reset(m);
                game_init(m);
                break;
        }
    } while (((by >= 0 && by < m->rows)
             && (bx  >= 0 && bx < m->cols)) 
             && m->numdefused < m->nummines && !m->gameover
             && tolower((char)move) != MOVE_QUIT);  

    if (m->gameover) gameover_handle(m);
    if (m->numdefused == m->nummines) win_handle(m);
}

int
cols_set(void)
{
    int cols;
    do {
        mvprintw(YMAX(stdscr)-4, 1, "Columns (Min = 5, Max = %d): ",
                ARRSPACE_X(XMAX(stdscr)) - 2);
        refresh();
        scanw("%d", &cols);
        refresh();
    } while (cols < 5 || cols > ARRSPACE_X(XMAX(stdscr)) - 2);
    return cols;    
}

int
rows_set(void)
{
    int rows;
    do {
        mvprintw(YMAX(stdscr)-3, 1, "Rows (Min = 5, Max = %d): ",
                ARRSPACE_Y(YMAX(stdscr)) - 3);
        refresh();
        scanw("%d", &rows);
        refresh();
    } while (rows < 5 || rows > ARRSPACE_Y(YMAX(stdscr)) - 3); 
    return rows;
}

int
nummines_set(int dimensions)
{
    int nmines;
    do {
        mvprintw(YMAX(stdscr)-2, 1, "Mines (Max = %d): ", dimensions-15);
        refresh();
        scanw("%d", &nmines);
        refresh();
    } while (nmines < 1 || nmines > dimensions-15);
    return nmines;
}

void
dispboard_init(struct Minecurses *m)
{
    BOARD_ALLOC(m->dispboard, m->rows, m->cols);
    dispboard_fill(m);
}

void
dispboard_fill(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            m->dispboard[i][j] = CELL_BLANK;
}

void
mineboard_init(struct Minecurses *m)
{
    BOARD_ALLOC(m->mineboard, m->rows, m->cols);
    mineboard_mines_place(m);
    mineboard_add_adj(m);
    mineboard_spaces_fill(m);
}

void
mineboard_mines_place(struct Minecurses *m)
{
    size_t i, r, c;
    srand(time(NULL));
    for (i = 0; i < m->nummines; i++) {
        r = rand() % m->rows;
        c = rand() % m->cols;
        m->mineboard[r][c] = CELL_MINE;
    }
}

void
mineboard_add_adj(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (!IS_MINE(m, i, j))
                m->mineboard[i][j] = mineboard_mines_count_adj(m, i, j) + '0';
}

uint8_t
mineboard_mines_count_adj(const struct Minecurses *m, int r, int c)
{
    uint8_t nadj = 0;
    if (!OUT_OF_BOUNDS(m, r, c-1)   && m->mineboard[r][c-1]   == CELL_MINE) nadj++; // North
    if (!OUT_OF_BOUNDS(m, r, c+1)   && m->mineboard[r][c+1]   == CELL_MINE) nadj++; // South
    if (!OUT_OF_BOUNDS(m, r+1, c)   && m->mineboard[r+1][c]   == CELL_MINE) nadj++; // East
    if (!OUT_OF_BOUNDS(m, r-1, c)   && m->mineboard[r-1][c]   == CELL_MINE) nadj++; // West
    if (!OUT_OF_BOUNDS(m, r+1, c-1) && m->mineboard[r+1][c-1] == CELL_MINE) nadj++; // North-East
    if (!OUT_OF_BOUNDS(m, r-1, c-1) && m->mineboard[r-1][c-1] == CELL_MINE) nadj++; // North-West
    if (!OUT_OF_BOUNDS(m, r+1, c+1) && m->mineboard[r+1][c+1] == CELL_MINE) nadj++; // South-East
    if (!OUT_OF_BOUNDS(m, r-1, c+1) && m->mineboard[r-1][c+1] == CELL_MINE) nadj++; // South-West
    return nadj;
}

void
mineboard_spaces_fill(struct Minecurses *m)
{
    size_t i, j;
    for (i = 0; i < m->rows; i++)
        for (j = 0; j < m->cols; j++)
            if (m->mineboard[i][j] != CELL_MINE && m->mineboard[i][j] == '0')
                m->mineboard[i][j] = '-';
}

void
board_navigate(struct Minecurses *m, int *mv)
{
    static int y = 1, x = 2;
    CURS_UPDATE(m, y, x);
    m->x = ARRSPACE_X(x);
    m->y = ARRSPACE_Y(y);
    refresh();
    mvget(m, mv, &y, &x);
}

void
boards_dealloc(struct Minecurses *m)
{
    if (!m->dispboard && !m->mineboard) {
        size_t i;
        for (i = 0; i < m->rows; i++) {
            free(m->dispboard[i]);
            free(m->mineboard[i]);
        }
        free(m->dispboard);
        free(m->mineboard);
    }
}

int
cell_open(struct Minecurses *m)
{
    cell_transfer(m);
    cell_reveal(m);
    return (m->dispboard[by][bx] == CELL_MINE);
}

void
cell_transfer(struct Minecurses *m)
{
    m->dispboard[by][bx] = m->mineboard[by][bx];
}

void
cell_reveal(const struct Minecurses *m)
{
    int y = m->y + 1;
    int x = GRIDSPACE_X(m->x);
    mvwaddch(m->gamewin, y, x, m->dispboard[by][bx]);
    wrefresh(m->gamewin);
}

void
flags_handle(struct Minecurses *m)
{
    if (m->dispboard[by][bx] == CELL_FLAGGED)
        m->dispboard[by][bx] = CELL_BLANK;
    else if (m->dispboard[by][bx] != CELL_FLAGGED
         && m->dispboard[by][bx] != CELL_BLANK)
        return;
    else m->dispboard[by][bx] = CELL_FLAGGED;
    cell_reveal(m);
}

void
mine_defuse(struct Minecurses *m)
{
    refresh();
    m->dispboard[by][bx] = m->mineboard[by][bx] = MINE_DEFUSED;
    cell_reveal(m);
}

#undef bx
#undef by

void
mvget(const struct Minecurses *m, int *mv, int *y, int *x)
{
    *mv = wgetch(m->gamewin);
    switch (*mv) {
        case 'k': case 'K': /* FALLTHROUGH */
        case 'w': case 'W':
            mvup(y);
            break;
        case 'j': case 'J': /* FALLTHROUGH */
        case 's': case 'S':
            mvdown(y, YMAX(m->gamewin));
            break;
        case 'h': case 'H': /* FALLTHROUGH */
        case 'a': case 'A':
            mvleft(x);
            break;
        case 'l': case 'L': /* FALLTHROUGH */
        case 'd': case 'D':
            mvright(x, XMAX(m->gamewin));
            break;
    }
}

void
mvup(int *y)
{
    (*y)--;
    if (*y < 1) *y = 1;
}

void
mvdown(int *y, int ymax)
{
    (*y)++;
    if (*y > ymax-2) *y = ymax-2;
}

void
mvleft(int *x)
{
    *x -= 3;
    if (*x < 2) *x = 2;
}

void
mvright(int *x, int xmax)
{
    *x += 3;
    if (*x > xmax-3) *x = xmax-3;
}

void
curses_init(void)
{
    initscr();
    noecho();
    cbreak();
}

void
board_print(const struct Minecurses *m)
{    
    size_t i, j, x, y = 1;
    grid_print(m);
    wattron(m->gamewin, A_BOLD);
    for (i = 0; i < m->rows; y++, i++)
        for (j = 0, x = 2; j < m->cols; j++, x += 3)
            mvwaddch(m->gamewin, y, x, m->dispboard[i][j]);
}

void
grid_print(const struct Minecurses *m)
{
    size_t i, j;
    wattroff(m->gamewin, A_BOLD);
    for (i = 1; i <= m->rows; i++) {
        wmove(m->gamewin, i, 1);
        for (j = 0; j < m->cols; j++)
            wprintw(m->gamewin, "[ ]");
    }
    wrefresh(m->gamewin);
}

WINDOW *
gamewin_init(int rows, int cols)
{
    int wrows  = GRIDSPACE_Y(rows);
    int wcols  = GRIDSPACE_X(cols);
    int wy     = CENTER(YMAX(stdscr), wrows);
    int wx     = CENTER(XMAX(stdscr), wcols);
    WINDOW *gw = newwin(wrows, wcols, wy, wx);
    wattron(gw, A_BOLD);
    box(gw, 0, 0);
    wrefresh(gw);
    wattroff(gw, A_BOLD);
    return gw;
}

void
menu_handle(const struct Minecurses *m)
{
    menu_options();
    box(m->gamewin, 0, 0);
    board_print(m);
}

void
menu_options(void)
{
    int w  = 33, h = 15;
    int wy = CENTER(YMAX(stdscr), h);
    int wx = CENTER(XMAX(stdscr), w);
    WINDOW *opts = newwin(h, w, wy, wx);
    werase(opts);
    box(opts, 0, 0);
    menu_fill(opts);
    wrefresh(opts);
    wgetch(opts);
    werase(opts);
    wrefresh(opts);
    delwin(opts);
}

void
menu_fill(WINDOW *opts)
{
    mvwprintw(opts, 1, 1,  "q    Quit");
    mvwprintw(opts, 2, 1,  "w/k  Move up");
    mvwprintw(opts, 3, 1,  "s/j  Move down");
    mvwprintw(opts, 4, 1,  "a/h  Move left");
    mvwprintw(opts, 5, 1,  "d/l  Move right");
    mvwprintw(opts, 6, 1,  "f    Flag cell");
    mvwprintw(opts, 7, 1,  "g    Defuse (if flagged only)");
    mvwprintw(opts, 8, 1,  "p    Pause music");
    mvwprintw(opts, 9, 1,  "+    Volume up");
    mvwprintw(opts, 10, 1, "-   Volume down");
    mvwprintw(opts, 11, 1, "[ENTER]/o Open cell");
    mvwprintw(opts, 13, 1, "Press any key to quit the menu");
}

void
session_info(const struct Minecurses *m)
{
    mvprintw(0, 0, "Current position: (%d, %d) ", m->x, m->y);
    mvprintw(0, XMID(stdscr) - strlen("Defused mines: x/x")/2,
            "Defused mines: %d/%d", m->numdefused, m->nummines);
    mvprintw(0, XMAX(stdscr) - strlen("m Controls"), "m Controls");
}

void
win_handle(const struct Minecurses *m)
{
    endscreen(m, GAME_WON);
    getchar();
    session_write(m, GAME_WON);
    score_write(m);
}

void
gameover_handle(const struct Minecurses *m)
{
    endscreen(m, GAME_LOST);
    getchar();
    erase();
    refresh();
    box(m->gamewin, 0, 0);
    board_print(m);
    wrefresh(m->gamewin);
    session_write(m, GAME_LOST);
}

void
endscreen(const struct Minecurses *m, const enum State state)
{
    wclear(m->gamewin);
    wrefresh(m->gamewin);
    attron(A_BOLD);
    switch (state) {
        case GAME_WON:
            mvprintw(YMID(stdscr)-2, XMID(stdscr)-11,
                    "You defused all the mines!");
            mvprintw(YMID(stdscr)-1, XMID(stdscr)-3, "You won :)");
            break;
        case GAME_LOST:
            mvprintw(YMID(stdscr)-2, XMID(stdscr)-24,
                    "You hit a mine! (or tried to defuse the wrong cell)");
            mvprintw(YMID(stdscr)-1, XMID(stdscr)-4, "Game over :(");
            break;
    }
    mvprintw(YMID(stdscr), XMID(stdscr)-11, "Press any key to continue");
    refresh();
    attroff(A_BOLD);
}

void
session_write(const struct Minecurses *m, const enum State state)
{
    size_t i, j;
    FILE *fsession = fopen(SESSION_PATH, "w");
    if (fsession == NULL) die();
    else {
        state == GAME_WON
            ? fprintf(fsession, "Mine hit at position (%d, %d)\n\n",
                    m->x+1, m->y+1)
            : fprintf(fsession, "Last mine defused at position (%d, %d)\n\n",
                    m->x+1, m->y+1);
        fprintf(fsession, "Board overview\n\n");
        for (i = 0; i < m->rows; i++) {
            for (j = 0; j < m->cols; j++)
                fprintf(fsession, "%c ", m->mineboard[i][j]);
            fprintf(fsession, "\n");
        }           
    }
    fclose(fsession);
}

void
score_write(const struct Minecurses *m)
{
    char *playername = playername_get();
    FILE *scorelog = fopen(SCORE_LOG_PATH, "a");
    if (scorelog == NULL) die();
    else {
        fprintf(scorelog, "%s,%d,%dx%d\n",
                playername, m->numdefused, m->cols, m->rows);
        clrtoeol();
        mvprintw(0, 0, "New score written to score log");
        refresh();
        getchar();
    }
    fclose(scorelog);
    free(playername);
}

char *
playername_get(void)
{   
    char buffer[48];
    char *playername;
    move(0, 0);
    echo();
    clrtoeol();
    printw("Your name: ");
    refresh();
    scanw("%s", buffer);
    noecho();
    refresh();
    playername = (char *)malloc(strlen(buffer) + 1);
    return (strcpy(playername, buffer));
}

void *
audio_play(void *tid)
{
    /*int threadid = (long)tid;*/
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096);
    Mix_Music *music = Mix_LoadMUS(AUDIO_PATH);

    Mix_PlayMusic(music, -1);
    while (Mix_PlayingMusic());
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
    return NULL;
}

void
audio_change_volume(char option)
{
    static int volume = MIX_MAX_VOLUME;
    switch (option) {
        case '+':
            if (volume == MIX_MAX_VOLUME) break;
            else Mix_VolumeMusic(volume += VOL_STEP);
            break;
        case '-':
            if (volume == 0) break;
            else Mix_VolumeMusic(volume -= VOL_STEP);
            break;
    }
}

void
die(void)
{
    mvprintw(0, 0, "Error, cannot allocate memory, exiting...");
    refresh();
    exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{   
#ifndef NCURSES_VERSION
    fprintf(stderr, "ncurses is needed in order to run this program.\n");
    return EXIT_FAILURE;
#endif /* NCURSES_VERSION */
    curses_init();
    struct Minecurses m;
    game_reset(&m);
    m.gamewin = gamewin_init(m.rows, m.cols);
    game_init(&m);

    pthread_t audiothread;
    long tid = 1;
    pthread_create(&audiothread, NULL, audio_play, (void *)tid);
    game_start(&m);
    
    pthread_cancel(audiothread);
    boards_dealloc(&m);
    delwin(m.gamewin);
    endwin();
    return EXIT_SUCCESS;
}
