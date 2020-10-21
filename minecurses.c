/* See LICENSE file for copyright and license details. */

#include <sys/types.h>

#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "defs.h"

struct Minecurses {
        WINDOW *gamewin;
        char  **dispboard;
        char  **mineboard;
        int     rows, cols;
        int     nummines;
        int     numdefused;
        int     move;
        int     x, y;
        int     gameover;
};

enum State {
        GAME_WON,
        GAME_LOST
};

static void     game_init(struct Minecurses *);
static void     game_reset(struct Minecurses *);
static void     game_start(struct Minecurses *);
static int      cols_set(void);
static int      rows_set(void);
static int      nummines_set(int);
static void     dispboard_fill(struct Minecurses *);
static void     mineboard_mines_place(struct Minecurses *);
static void     mineboard_add_adj(struct Minecurses *);
static u_int8_t mineboard_count_adj(const struct Minecurses *, int, int);
static void     mineboard_spaces_fill(struct Minecurses *);
static void     boards_alloc(struct Minecurses *);
static void     boards_dealloc(struct Minecurses *);
static void     board_navigate(struct Minecurses *);
static int      cell_open(struct Minecurses *);
static void     cell_transfer(struct Minecurses *);
static void     cell_reveal(const struct Minecurses *);
static void     flags_handle(struct Minecurses *);
static void     mine_defuse(struct Minecurses *);
static void     mvget(struct Minecurses *, int *, int *);
static void     mvup(int *);
static void     mvdown(int *, int);
static void     mvleft(int *);
static void     mvright(int *, int);
static void     board_print(const struct Minecurses *);
static void     grid_print(const struct Minecurses *);
static WINDOW  *gamewin_init(int, int);
static void     menu_handle(const struct Minecurses *);
static void     menu_options(void);
static void     menu_fill(struct _win_st *);
static void     session_info(const struct Minecurses *);
static void     win_handle(const struct Minecurses *);
static void     gameover_handle(const struct Minecurses *);
static void     endscreen(const struct Minecurses *, const enum State);
static void     session_write(const struct Minecurses *);
static void     score_write(const struct Minecurses *);
static char    *playername_get(void);
static void    *audio_play(void *);
static void     audio_change_volume(char);
static void     die(void);

void
game_init(struct Minecurses *m)
{
        if (m->gamewin == NULL)
                m->gamewin = gamewin_init(m->rows, m->cols);
        boards_alloc(m);
        dispboard_fill(m);
        MINEBOARD_INIT(m);
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
        bx = by = 0;
        m->numdefused = 0;
        m->gameover = FALSE;

        do {
                erase();
                delwin(m->gamewin);
                refresh();
                m->gamewin = gamewin_init(m->rows, m->cols);
                board_print(m);
                session_info(m);
                board_navigate(m);

                switch (tolower((char)m->move)) {
                case MOVE_ENTER:         /* FALLTHROUGH */
                case MOVE_OPEN_CELL:
                        m->gameover = cell_open(m);
                        break;
                case MOVE_FLAG_CELL:
                        flags_handle(m);
                        break;
                case MOVE_DEFUSE_CELL:
                        if (m->dispboard[by][bx] == CELL_FLAGGED
                        &&  m->mineboard[by][bx] == CELL_MINE) {
                                m->numdefused++;
                                mine_defuse(m);
                        }
                        else if (m->dispboard[by][bx] == CELL_FLAGGED
                             &&  m->mineboard[by][bx] != CELL_MINE)
                                m->gameover = TRUE;              
                        break;
                case MOVE_PAUSE_AUDIO:
                        AUDIO_PAUSE();
                        break;
                case MOVE_VOLUME_UP:     /* FALLTHROUGH */
                case MOVE_VOLUME_DOWN:
                        audio_change_volume(m->move);
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
              && m->numdefused < m->nummines
              && !m->gameover
              && tolower((char)m->move) != MOVE_QUIT);

        if (m->gameover)
                gameover_handle(m);
        if (m->numdefused == m->nummines)
                win_handle(m);
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
dispboard_fill(struct Minecurses *m)
{
        size_t i, j;

        for (i = 0; i < m->rows; i++)
                for (j = 0; j < m->cols; j++)
                    m->dispboard[i][j] = CELL_BLANK;
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
                                m->mineboard[i][j] = mineboard_count_adj(m, i, j) + '0';
}

// fix styling
u_int8_t
mineboard_count_adj(const struct Minecurses *m, int r, int c)
{
        u_int8_t nadj = 0;

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
                        if (m->mineboard[i][j] != CELL_MINE
                        &&  m->mineboard[i][j] == '0')
                                m->mineboard[i][j] = '-';
}

void
board_navigate(struct Minecurses *m)
{
        static int y = 1, x = 2;

        CURS_UPDATE(m, y, x);
        m->x = ARRSPACE_X(x);
        m->y = ARRSPACE_Y(y);
        refresh();
        mvget(m, &y, &x);
}

void
boards_alloc(struct Minecurses *m)
{
        size_t i = 0;

        m->dispboard = malloc(m->rows * sizeof(char *));
        m->mineboard = malloc(m->rows * sizeof(char *));
        for (; i < m->rows; i++) {
                m->dispboard[i] = malloc(m->cols);
                m->mineboard[i] = malloc(m->cols);
        }
        if (!m->dispboard || !m->mineboard)
                die();
}

void
boards_dealloc(struct Minecurses *m)
{
        size_t i = 0;

        if (!m->dispboard && !m->mineboard) {
            for (; i < m->rows; i++) {
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
             &&  m->dispboard[by][bx] != CELL_BLANK)
                return;
        else
                m->dispboard[by][bx] = CELL_FLAGGED;
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
mvget(struct Minecurses *m, int *y, int *x)
{
        switch (m->move = wgetch(m->gamewin)) {
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
        if (--(*y) < 1)
                *y = 1;
}

void
mvdown(int *y, int ymax)
{
        if (++(*y) > ymax-2)
                *y = ymax-2;
}

void
mvleft(int *x)
{
        *x -= 3;
        if (*x < 2)
                *x = 2;
}

void
mvright(int *x, int xmax)
{
        *x += 3;
        if (*x > xmax-3)
                *x = xmax-3;
}

void
board_print(const struct Minecurses *m)
{    
        size_t i, j, x, y;

        grid_print(m);
        wattron(m->gamewin, A_BOLD);
        for (i = 0, y = 1; i < m->rows; y++, i++)
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
                        wprintw(m->gamewin, GRID_BOX);
        }
        wrefresh(m->gamewin);
}

WINDOW *
gamewin_init(int rows, int cols)
{
        int wr, wc, wy, wx;
        WINDOW *gw;
        
        wr = GRIDSPACE_Y(rows);
        wc = GRIDSPACE_X(cols);
        wy = CENTER(YMAX(stdscr), wr);
        wx = CENTER(XMAX(stdscr), wc);
        gw = newwin(wr, wc, wy, wx);
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
        int w, h, wy, wx;
        WINDOW *opts;

        w  = 33;
        h = 15;
        wy = CENTER(YMAX(stdscr), h);
        wx = CENTER(XMAX(stdscr), w);
        opts = newwin(h, w, wy, wx);
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
        mvwprintw(opts, 1, 1,  OPT_QUIT);
        mvwprintw(opts, 2, 1,  OPT_MOVE_UP);
        mvwprintw(opts, 3, 1,  OPT_MOVE_DOWN);
        mvwprintw(opts, 4, 1,  OPT_MOVE_LEFT);
        mvwprintw(opts, 5, 1,  OPT_MOVE_RIGHT);
        mvwprintw(opts, 6, 1,  OPT_FLAG_CELL);
        mvwprintw(opts, 7, 1,  OPT_DEFUSE);
        mvwprintw(opts, 8, 1,  OPT_PAUSE_MUS);
        mvwprintw(opts, 9, 1,  OPT_VOL_UP);
        mvwprintw(opts, 10, 1, OPT_VOL_DOWN);
        mvwprintw(opts, 11, 1, OPT_OPEN_CELL);
        mvwprintw(opts, 13, 1, MSG_QUIT_MENU);
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
        session_write(m);
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
        session_write(m);
}

void
endscreen(const struct Minecurses *m, const enum State state)
{
        wclear(m->gamewin);
        wrefresh(m->gamewin);
        attron(A_BOLD);
        switch (state) {
        case GAME_WON:
                mvprintw(YMID(stdscr)-2, XMID(stdscr)-11, MSG_WIN1);
                mvprintw(YMID(stdscr)-1, XMID(stdscr)-3, MSG_WIN2);
                break;
        case GAME_LOST:
                mvprintw(YMID(stdscr)-2, XMID(stdscr)-24, MSG_LOSE1);
                mvprintw(YMID(stdscr)-1, XMID(stdscr)-4, MSG_LOSE2);
                break;
        }
        mvprintw(YMID(stdscr), XMID(stdscr)-11, MSG_CONT);
        refresh();
        attroff(A_BOLD);
}

void
session_write(const struct Minecurses *m)
{
        size_t i, j;
        FILE *fsession;

        if ((fsession = fopen(SESSION_PATH, "w")) == NULL)
                die();
        else {
                fprintf(fsession, "Last mine found at position (%d, %d)\n\n",
                        m->x+1, m->y+1);
                fputs("Board overview\n\n", fsession);
                for (i = 0; i < m->rows; i++) {
                        for (j = 0; j < m->cols; j++)
                                fprintf(fsession, "%c ", m->mineboard[i][j]);
                        fputs("\n", fsession);
                }           
        }
        fclose(fsession);
}

void
score_write(const struct Minecurses *m)
{
        char *playername;
        FILE *scorelog;

        if ((scorelog = fopen(SCORE_LOG_PATH, "a"))  == NULL)
                die();
        else {
                playername = playername_get();
                fprintf(scorelog, "%s,%d,%dx%d\n",
                        playername, m->numdefused, m->cols, m->rows);
                clrtoeol();
                mvprintw(0, 0, MSG_NEW_SCORE);
                refresh();
                getchar();
        }
        fclose(scorelog);
        free(playername);
}

char *
playername_get(void)
{   
        char buffer[48], *playername;

        move(0, 0);
        echo();
        clrtoeol();
        printw("Your name: ");
        refresh();
        scanw("%s", buffer);
        noecho();
        refresh();
        playername = malloc(strlen(buffer) + 1);
        return (strcpy(playername, buffer));
}

void *
audio_play(void *tid)
{
        Mix_Music *music;

        /*int threadid = (long)tid;*/
        SDL_Init(SDL_INIT_AUDIO);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096);
        music = Mix_LoadMUS(AUDIO_PATH);

        Mix_PlayMusic(music, -1);
        while (Mix_PlayingMusic())
                ;
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
        case MOVE_VOLUME_UP:
                if (volume == MIX_MAX_VOLUME)
                        break;
                else
                        Mix_VolumeMusic(volume += VOL_STEP);
                break;
        case MOVE_VOLUME_DOWN:
                if (volume == 0)
                        break;
                else
                        Mix_VolumeMusic(volume -= VOL_STEP);
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
main(int argc, char *argv[])
{
#ifndef NCURSES_VERSION
        fputs("ncurses is needed in order to run this program.\n", stderr);
        return EXIT_FAILURE;
#endif /* NCURSES_VERSION */
        struct Minecurses m;
        long tid;

        CURSES_INIT();
        game_reset(&m);
        game_init(&m);

        pthread_t audiothread;
        tid = 1;
        pthread_create(&audiothread, NULL, audio_play, (void *)tid);
        game_start(&m);
    
        pthread_cancel(audiothread);
        boards_dealloc(&m);
        delwin(m.gamewin);
        endwin();
        return EXIT_SUCCESS;
}
