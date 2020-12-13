/* See LICENSE file for copyright and license details. */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "defs.h"

typedef struct {
        WINDOW   *gamewin;
        char    **dispboard;
        char    **mineboard;
        int       rows;
        int       cols;
        int       nmines;
        int       ndefused;
        int       move;
        int       x;
        int       y;
        int       gameover;
} Minecurses;

static void      gamereset(Minecurses *);
static void      gamerestart(Minecurses *);
static void      gamestart(Minecurses *);
static int       valset(int, const char *, int, int);
static int       adjcount(const Minecurses *, int, int);
static void      boardsdealloc(Minecurses *);
static void      cellreveal(const Minecurses *);
static void      boardprint(const Minecurses *);
static WINDOW   *gamewininit(int, int);
static void      menuopts(void);
static void      endscreen(Minecurses *, int);
static void     *emalloc(size_t);

static void
gamereset(Minecurses *m)
{
        size_t i, j, r, c;

        echo();
        m->cols = valset(4, MSG_COLS, 5, ARRSPACE_X(XMAX(stdscr)) - 2);
        m->rows = valset(3, MSG_ROWS, 5, ARRSPACE_Y(YMAX(stdscr)) - 3);
        m->nmines = valset(2, MSG_MINES, 1, m->rows * m->cols - 15);
        m->x = m->y = 0;
        m->ndefused = 0;
        m->gameover = 0;
        noecho();

        menuopts();
        if (m->gamewin == NULL)
                m->gamewin = gamewininit(m->rows, m->cols);

        /* allocate memory for the boards */
        m->dispboard = emalloc(m->rows * sizeof(char *));
        m->mineboard = emalloc(m->rows * sizeof(char *));
        for (i = 0; i < m->rows; i++) {
                m->dispboard[i] = emalloc(m->cols);
                m->mineboard[i] = emalloc(m->cols);
        }

        /* place mines */
        srand(time(NULL));
        for (i = 0; i < m->nmines; i++) {
                r = rand() % m->rows;
                c = rand() % m->cols;
                m->mineboard[r][c] = CELL_MINE;
        }

        /* TODO: do it in one loop */
        /* add numbers */
        for (i = 0; i < m->rows; i++) {
                for (j = 0; j < m->cols; j++) {
                        if (!IS_MINE(m, i, j))
                                m->mineboard[i][j] = adjcount(m, i, j) + '0';
                        /* in the meantime, initialize dispboard */
                        m->dispboard[i][j] = CELL_BLANK;
                }
        }

        /* fill spaces */
        for (i = 0; i < m->rows; i++)
                for (j = 0; j < m->cols; j++)
                        if (!IS_MINE(m, i, j) &&  m->mineboard[i][j] == '0')
                                m->mineboard[i][j] = '-';
}

static void
gamerestart(Minecurses *m)
{
        boardsdealloc(m);
        gamereset(m);
}

#define bx m->x
#define by m->y

static void
gamestart(Minecurses *m)
{
        static int y = 1, x = 2;

        for (;;) {
                erase();
                // is this necessary?
                delwin(m->gamewin);
                m->gamewin = gamewininit(m->rows, m->cols);
                refresh();
                boardprint(m);

                CURS_UPDATE(m, y, x);
                bx = ARRSPACE_X(x);
                by = ARRSPACE_Y(y);

                /* session info */
                mvprintw(0, 0, MSG_CURPOS, bx, by);
                mvprintw(0, XMID(stdscr) - (strlen(MSG_NDEFUSED) - 2) / 2,
                         MSG_NDEFUSED, m->ndefused, m->nmines);
                mvprintw(0, XMAX(stdscr) - strlen(OPT_CTRLS), OPT_CTRLS);

                refresh();

                /* handle moves */
                switch (m->move = wgetch(m->gamewin)) {
                case MOVE_UP_NORM:      /* FALLTHROUGH */
                case MOVE_UP_VIM:
                        if (--y < 1)
                                y = 1;
                        break;
                case MOVE_DOWN_NORM:    /* FALLTHROUGH */
                case MOVE_DOWN_VIM:
                        if (++y > XMAX(m->gamewin) - 2)
                                y = YMAX(m->gamewin) - 2;
                        break;
                case MOVE_LEFT_NORM:    /* FALLTHROUGH */
                case MOVE_LEFT_VIM:
                        x -= 3;
                        if (x < 2)
                                x = 2;
                        break;
                case MOVE_RIGHT_NORM:   /* FALLTHROUGH */
                case MOVE_RIGHT_VIM:
                        x += 3;
                        if (x > XMAX(m->gamewin) - 3)
                                x = XMAX(m->gamewin) - 3;
                        break;
                case MOVE_ENTER:        /* FALLTHROUGH */
                case MOVE_OPEN_CELL:
                        m->dispboard[by][bx] = m->mineboard[by][bx];
                        m->gameover = IS_MINE(m, by, bx);
                        cellreveal(m);
                        break;
                case MOVE_FLAG_CELL:
                        if (IS_FLAGGED(m, by, bx))
                                m->dispboard[by][bx] = CELL_BLANK;
                        else if (!IS_FLAGGED(m, by, bx) && !IS_BLANK(m, by, bx))
                                break;
                        else
                                m->dispboard[by][bx] = CELL_FLAGGED;
                        cellreveal(m);
                        break;
                case MOVE_DEFUSE_CELL:
                        if (IS_FLAGGED(m, by, bx) &&  IS_MINE(m, by, bx)) {
                                m->ndefused++;
                                m->dispboard[by][bx] = MINE_DEFUSED;
                                m->mineboard[by][bx] = MINE_DEFUSED;
                                cellreveal(m);
                        } else if (IS_FLAGGED(m, by, bx) && !IS_MINE(m, by, bx))
                                m->gameover = 1;              
                        break;
                case MOVE_OPEN_MENU:
                        menuopts();
                        box(m->gamewin, 0, 0);
                        boardprint(m);
                        break;
                case MOVE_RESTART:
                        gamerestart(m);
                        break;
                }

                if (OUT_OF_BOUNDS(m, by, bx)
                ||  m->ndefused > m->nmines
                ||  m->gameover
                ||  m->move == MOVE_QUIT)
                        break;
        }

        if (m->gameover)
                endscreen(m, GAME_LOST);
        else if (m->ndefused == m->nmines)
                endscreen(m, GAME_WON);
}

#undef bx
#undef by

static int
valset(int offy, const char *msg, int min, int max)
{
        int val;

        do {
                mvprintw(YMAX(stdscr)-offy, 1, msg, min, max);
                scanw("%d", &val);
        } while (val < min || val > max);
        return val;
}

static int
adjcount(const Minecurses *m, int r, int c)
{
        int nadj = 0;

        if (!OUT_OF_BOUNDS(m, r, c-1)   && IS_MINE(m, r, c-1))   nadj++; // north
        if (!OUT_OF_BOUNDS(m, r, c+1)   && IS_MINE(m, r, c+1))   nadj++; // south
        if (!OUT_OF_BOUNDS(m, r+1, c)   && IS_MINE(m, r+1, c))   nadj++; // east
        if (!OUT_OF_BOUNDS(m, r-1, c)   && IS_MINE(m, r-1, c))   nadj++; // west
        if (!OUT_OF_BOUNDS(m, r+1, c-1) && IS_MINE(m, r+1, c-1)) nadj++; // north-east
        if (!OUT_OF_BOUNDS(m, r-1, c-1) && IS_MINE(m, r-1, c-1)) nadj++; // north-west
        if (!OUT_OF_BOUNDS(m, r+1, c+1) && IS_MINE(m, r+1, c+1)) nadj++; // south-east
        if (!OUT_OF_BOUNDS(m, r-1, c+1) && IS_MINE(m, r-1, c+1)) nadj++; // south-west

        return nadj;
}

static void
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

static void
cellreveal(const Minecurses *m)
{
        int y = m->y + 1;
        int x = SCRSPACE_X(m->x);

        mvwaddch(m->gamewin, y, x, m->dispboard[m->y][m->x]);
}

static void
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

static WINDOW *
gamewininit(int rows, int cols)
{
        WINDOW *gw;
        int wr, wc, wy, wx;
        
        wr = SCRSPACE_Y(rows);
        wc = SCRSPACE_X(cols);
        wy = CENTER(YMAX(stdscr), wr);
        wx = CENTER(XMAX(stdscr), wc);
        gw = newwin(wr, wc, wy, wx);
        wattron(gw, A_BOLD);
        box(gw, 0, 0);
        wattroff(gw, A_BOLD);

        return gw;
}

static void
menuopts(void)
{
        WINDOW *opts;
        int w, h, wy, wx;

        w = 36;
        h = 13;
        wy = CENTER(YMAX(stdscr), h);
        wx = CENTER(XMAX(stdscr), w);
        opts = newwin(h, w, wy, wx);
        box(opts, 0, 0);

        /* fill menu */
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

        wgetch(opts);
        delwin(opts);
}

static void
endscreen(Minecurses *m, int state)
{
        curs_set(0);
        wclear(m->gamewin);
        wrefresh(m->gamewin);
        attron(A_BOLD);
        switch (state) {
        case GAME_WON:
                mvprintw(YMID(stdscr)-2, XMID(stdscr)-11, MSG_WIN_1);
                mvprintw(YMID(stdscr)-1, XMID(stdscr)-3, MSG_WIN_2);
                break;
        case GAME_LOST:
                mvprintw(YMID(stdscr)-2, XMID(stdscr)-24, MSG_LOSE_1);
                mvprintw(YMID(stdscr)-1, XMID(stdscr)-4, MSG_LOSE_2);
                // TODO: print mine board
                break;
        }
        mvprintw(YMID(stdscr), XMID(stdscr)-11, MSG_CONT);
        refresh();
        attroff(A_BOLD);
        getchar();
        // TODO: restart option
}

static void *
emalloc(size_t nb)
{
        void *p;

        if ((p = malloc(nb)) == NULL) {
                fputs("minecurses: emalloc: cannot allocate memory\n", stderr);
                exit(EXIT_FAILURE);
        }
        return p;
}

int
main(int argc, char *argv[])
{
        Minecurses m;

        if (!initscr()) {
                fputs("minecurses: initscr: cannot initialize ncurses\n", stderr);
                return 1;
        }
        noecho();
        cbreak();

        gamereset(&m);
        gamestart(&m);

        boardsdealloc(&m);
        delwin(m.gamewin);
        endwin();

        return EXIT_SUCCESS;
}
