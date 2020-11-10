/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "defs.h"

typedef struct {
        WINDOW   *gamewin;
        char    **dispboard;
        char    **mineboard;
        int       rows, cols;
        int       nmines;
        int       ndefused;
        int       move;
        int       x, y;
        int       gameover;
} Minecurses;

static void      gamereset(Minecurses *);
static void      gamestart(Minecurses *);
static int       valset(int, const char *, int, int);
static void      dispboardfill(Minecurses *);
static void      minesplace(Minecurses *);
static void      adjadd(Minecurses *);
static int       adjcount(const Minecurses *, int, int);
static void      spacesfill(Minecurses *);
static void      boardsalloc(Minecurses *);
static void      boardsdealloc(Minecurses *);
static void      boardnav(Minecurses *);
static int       cellopen(Minecurses *);
static void      celltransfer(Minecurses *);
static void      cellreveal(const Minecurses *);
static void      flagshandle(Minecurses *);
static void      minedefuse(Minecurses *);
static void      mvget(Minecurses *, int *, int *);
static void      mvup(int *);
static void      mvdown(int *, int);
static void      mvleft(int *);
static void      mvright(int *, int);
static void      boardprint(const Minecurses *);
static WINDOW   *gamewininit(int, int);
static void      menuhandle(const Minecurses *);
static void      menuopts(void);
static void      menufill(WINDOW *);
static void      sessioninfo(const Minecurses *);
static void      endscreen(const Minecurses *, int);
static void      cleanup(Minecurses *);
static void     *emalloc(size_t);

void
gamereset(Minecurses *m)
{
        echo();
        m->cols = valset(4, MSG_COLS, 5, ARRSPACE_X(XMAX(stdscr)) - 2);
        m->rows = valset(3, MSG_ROWS, 5, ARRSPACE_Y(YMAX(stdscr)) - 3);
        m->nmines = valset(2, MSG_MINES, 1, m->rows * m->cols - 15);
        m->ndefused = m->x = m->y = 0;
        m->gameover = FALSE;
        noecho();
        menuopts();
        if (m->gamewin == NULL)
                m->gamewin = gamewininit(m->rows, m->cols);
        boardsalloc(m);
        dispboardfill(m);
        MINEBOARD_INIT(m);
}

#define bx m->x
#define by m->y

void
gamestart(Minecurses *m)
{
        bx = by = 0;
        m->ndefused = 0;
        m->gameover = FALSE;

        do {
                erase();
                // is this necessary?
                delwin(m->gamewin);
                m->gamewin = gamewininit(m->rows, m->cols);
                refresh();
                boardprint(m);
                sessioninfo(m);
                boardnav(m);

                switch (tolower((char)m->move)) {
                case MOVE_ENTER:         /* FALLTHROUGH */
                case MOVE_OPEN_CELL:
                        m->gameover = cellopen(m);
                        break;
                case MOVE_FLAG_CELL:
                        flagshandle(m);
                        break;
                case MOVE_DEFUSE_CELL:
                        if (m->dispboard[by][bx] == CELL_FLAGGED
                        &&  m->mineboard[by][bx] == CELL_MINE) {
                                m->ndefused++;
                                minedefuse(m);
                        } else if (m->dispboard[by][bx] == CELL_FLAGGED
                               &&  m->mineboard[by][bx] != CELL_MINE)
                                m->gameover = TRUE;              
                        break;
                case MOVE_OPEN_MENU:
                        menuhandle(m);
                        break;
                case MOVE_RESTART:
                        GAME_RESTART(m);
                        break;
                }
        } while (!OUT_OF_BOUNDS(m, by, bx)
              && m->ndefused < m->nmines
              && !m->gameover
              && tolower((char)m->move) != MOVE_QUIT);

        if (m->gameover)
                endscreen(m, GAME_LOST);
        if (m->ndefused == m->nmines)
                endscreen(m, GAME_WON);
}

int
valset(int offy, const char *msg, int min, int max)
{
        int val;

        do {
                mvprintw(YMAX(stdscr)-offy, 1, msg, min, max);
                scanw("%d", &val);
        } while (val < min || val > max);
        return val;
}

void
dispboardfill(Minecurses *m)
{
        size_t i, j;

        for (i = 0; i < m->rows; i++)
                for (j = 0; j < m->cols; j++)
                    m->dispboard[i][j] = CELL_BLANK;
}

void
minesplace(Minecurses *m)
{
        size_t i, r, c;

        srand(time(NULL));
        for (i = 0; i < m->nmines; i++) {
                r = rand() % m->rows;
                c = rand() % m->cols;
                m->mineboard[r][c] = CELL_MINE;
        }
}

void
adjadd(Minecurses *m)
{
        size_t i, j;

        for (i = 0; i < m->rows; i++)
                for (j = 0; j < m->cols; j++)
                        if (!IS_MINE(m, i, j))
                                m->mineboard[i][j] = adjcount(m, i, j) + '0';
}

int
adjcount(const Minecurses *m, int r, int c)
{
        int nadj = 0;

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
spacesfill(Minecurses *m)
{
        size_t i, j;

        for (i = 0; i < m->rows; i++)
                for (j = 0; j < m->cols; j++)
                        if (!IS_MINE(m, i, j) &&  m->mineboard[i][j] == '0')
                                m->mineboard[i][j] = '-';
}

void
boardnav(Minecurses *m)
{
        static int y = 1, x = 2;

        CURS_UPDATE(m, y, x);
        m->x = ARRSPACE_X(x);
        m->y = ARRSPACE_Y(y);
        refresh();
        mvget(m, &y, &x);
}

void
boardsalloc(Minecurses *m)
{
        size_t i = 0;

        m->dispboard = emalloc(m->rows * sizeof(char *));
        m->mineboard = emalloc(m->rows * sizeof(char *));
        for (; i < m->rows; i++) {
                m->dispboard[i] = emalloc(m->cols);
                m->mineboard[i] = emalloc(m->cols);
        }
}

void
boardsdealloc(Minecurses *m)
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
cellopen(Minecurses *m)
{
        celltransfer(m);
        cellreveal(m);
        return (m->dispboard[by][bx] == CELL_MINE);
}

void
celltransfer(Minecurses *m)
{
        m->dispboard[by][bx] = m->mineboard[by][bx];
}

void
cellreveal(const Minecurses *m)
{
        int y = m->y + 1;
        int x = GRIDSPACE_X(m->x);

        mvwaddch(m->gamewin, y, x, m->dispboard[by][bx]);
}

void
flagshandle(Minecurses *m)
{
        if (m->dispboard[by][bx] == CELL_FLAGGED)
                m->dispboard[by][bx] = CELL_BLANK;
        else if (m->dispboard[by][bx] != CELL_FLAGGED
             &&  m->dispboard[by][bx] != CELL_BLANK)
                return;
        else
                m->dispboard[by][bx] = CELL_FLAGGED;
        cellreveal(m);
}

void
minedefuse(Minecurses *m)
{
        m->dispboard[by][bx] = m->mineboard[by][bx] = MINE_DEFUSED;
        cellreveal(m);
}

#undef bx
#undef by

void
mvget(Minecurses *m, int *y, int *x)
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
boardprint(const Minecurses *m)
{    
        size_t i, j, x, y;

        wattroff(m->gamewin, A_BOLD);
        for (i = 1; i <= m->rows; i++) {
                wmove(m->gamewin, i, 1);
                for (j = 0; j < m->cols; j++)
                        wprintw(m->gamewin, GRID_BOX);
        }
        wattron(m->gamewin, A_BOLD);
        for (i = 0, y = 1; i < m->rows; i++, y++)
                for (j = 0, x = 2; j < m->cols; j++, x += 3)
                        mvwaddch(m->gamewin, y, x, m->dispboard[i][j]);
}

WINDOW *
gamewininit(int rows, int cols)
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
        wattroff(gw, A_BOLD);
        return gw;
}

void
menuhandle(const Minecurses *m)
{
        menuopts();
        box(m->gamewin, 0, 0);
        boardprint(m);
}

void
menuopts(void)
{
        int w, h, wy, wx;
        WINDOW *opts;

        w = 36;
        h = 13;
        wy = CENTER(YMAX(stdscr), h);
        wx = CENTER(XMAX(stdscr), w);
        opts = newwin(h, w, wy, wx);
        box(opts, 0, 0);
        menufill(opts);
        wgetch(opts);
        delwin(opts);
}

void
menufill(WINDOW *opts)
{
        mvwprintw(opts, 1, 1, OPT_QUIT);
        mvwprintw(opts, 2, 1, OPT_RESTART);
        mvwprintw(opts, 3, 1, OPT_MOVE_UP);
        mvwprintw(opts, 4, 1, OPT_MOVE_DOWN);
        mvwprintw(opts, 5, 1, OPT_MOVE_LEFT);
        mvwprintw(opts, 6, 1, OPT_MOVE_RIGHT);
        mvwprintw(opts, 7, 1, OPT_FLAG_CELL);
        mvwprintw(opts, 8, 1, OPT_DEFUSE);
        mvwprintw(opts, 9, 1, OPT_OPEN_CELL);
        mvwprintw(opts, 11, 1, MSG_QUIT_MENU);
}

void
sessioninfo(const Minecurses *m)
{
        mvprintw(0, 0, MSG_CURPOS, m->x, m->y);
        mvprintw(0, XMID(stdscr) - (strlen(MSG_NDEFUSED) - 2) / 2,
                 MSG_NDEFUSED, m->ndefused, m->nmines);
        mvprintw(0, XMAX(stdscr) - strlen(OPT_CTRLS), OPT_CTRLS);
}

void
endscreen(const Minecurses *m, int state)
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
        getchar();
        // restart option
}

void
cleanup(Minecurses *m)
{
        boardsdealloc(m);
        delwin(m->gamewin);
        endwin();
}

void *
emalloc(size_t nb)
{
        void *p;

        if ((p = malloc(nb)) == NULL) {
                fputs("Error, cannot allocate memory, exiting...\n", stderr);
                exit(EXIT_FAILURE);
        }
        return p;
}

int
main(int argc, char *argv[])
{
#ifndef NCURSES_VERSION
        fputs("ncurses is needed in order to run this program.\n", stderr);
        return EXIT_FAILURE;
#endif /* NCURSES_VERSION */
        Minecurses m;

        CURSES_INIT();
        gamereset(&m);
        gamestart(&m);
        cleanup(&m);
        return EXIT_SUCCESS;
}
