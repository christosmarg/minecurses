#include "gameplay.h"

#define bx m->x
#define by m->y

static void  board_print(const struct Minecurses *);
static void  grid_print(const struct Minecurses *);
static int   open_cell(struct Minecurses *);
static void  handle_flags(struct Minecurses *);
static void  defuse_mine(struct Minecurses *);
static void  transfer(struct Minecurses *);
static void  reveal(const struct Minecurses *);
static int   is_defused(const struct Minecurses *);
static void  handle_menu(const struct Minecurses *);
static void  handle_gameover(const struct Minecurses *);
static void  handle_win(const struct Minecurses *);
static void  navigate(struct Minecurses *, int *);
static void  getmv(const struct Minecurses *, int *, int *, int *);
static void  mvup(int *);
static void  mvdown(int *, int);
static void  mvleft(int *);
static void  mvright(int *, int);
static void  curs_update(const struct Minecurses *, int, int);
static void  session_info(const struct Minecurses *);
static void  endscreen(const struct Minecurses *, const State);

void
gmpl_start(struct Minecurses *m)
{
    m->x = m->y = 0;
    m->gameover = FALSE;
    m->numdefused = 0;
    int move;

    do {
        erase();
        delwin(m->gamewin);
        refresh();
        m->gamewin = wins_gamewin_init(m->rows, m->cols);
        board_print(m);
        session_info(m);
        navigate(m, &move);

        switch (tolower((char)move))
        {
            case MOVE_ENTER:         /* FALLTHROUGH */
            case MOVE_OPEN_CELL:
                m->gameover = open_cell(m);
                break;
            case MOVE_FLAG_CELL:
                handle_flags(m);
                break;
            case MOVE_DEFUSE_CELL:
                if (m->dispboard[by][bx] == CELL_FLAGGED && m->mineboard[by][bx] == MINE)
                {
                    m->numdefused++;
                    defuse_mine(m);
                }
                else if (m->dispboard[by][bx] == CELL_FLAGGED && m->mineboard[by][bx] != MINE)
                    m->gameover = TRUE;              
                break;
            case MOVE_PAUSE_AUDIO:
                audio_pause();
                break;
            case MOVE_VOLUME_UP:     /* FALLTHROUGH */
            case MOVE_VOLUME_DOWN:
                audio_change_volume(move);
                break;
            case MOVE_OPEN_MENU:
                handle_menu(m);
                break;
            case MOVE_RESTART:
                util_dealloc_boards(m);
                util_reset(m);
                util_game_init(m);
                break;
        }
    } while (((by >= 0 && by < m->rows) &&
             (bx  >= 0 && bx < m->cols))  &&
             m->numdefused < m->nummines && !m->gameover &&
             tolower((char)move) != MOVE_QUIT);  

    if (m->gameover) handle_gameover(m);
    if (m->numdefused == m->nummines) handle_win(m);
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
    for (i = 1; i <= m->rows; i++)
    {
        wmove(m->gamewin, i, 1);
        for (j = 0; j < m->cols; j++)
            wprintw(m->gamewin, "[ ]");
    }
    wrefresh(m->gamewin);
}

int
open_cell(struct Minecurses *m)
{
    transfer(m);
    reveal(m);
    return (m->dispboard[by][bx] == MINE);
}

void
handle_flags(struct Minecurses *m)
{
    if (m->dispboard[by][bx] == CELL_FLAGGED)
        m->dispboard[by][bx] = BLANK;
    else if (m->dispboard[by][bx] != CELL_FLAGGED &&
             m->dispboard[by][bx] != BLANK)
        return;
    else m->dispboard[by][bx] = CELL_FLAGGED;
    reveal(m);
}

void
defuse_mine(struct Minecurses *m)
{
    refresh();
    m->dispboard[by][bx] = m->mineboard[by][bx] = MINE_DEFUSED;
    reveal(m);
}

void
transfer(struct Minecurses *m)
{
    m->dispboard[by][bx] = m->mineboard[by][bx];
}

void
reveal(const struct Minecurses *m)
{
    int y = m->y + 1;
    int x = GRIDSPACE_X(m->x);
    mvwaddch(m->gamewin, y, x, m->dispboard[by][bx]);
    wrefresh(m->gamewin);
}

int
is_defused(const struct Minecurses *m)
{
    return (m->dispboard[by][bx] == MINE_DEFUSED);
}

void
handle_menu(const struct Minecurses *m)
{
    wins_menu_options();
    box(m->gamewin, 0, 0);
    board_print(m);
}

void
handle_gameover(const struct Minecurses *m)
{
    endscreen(m, GAME_LOST);
    getchar();
    erase();
    refresh();
    box(m->gamewin, 0, 0);
    board_print(m);
    wrefresh(m->gamewin);
    util_session_write(m, GAME_LOST);
}

void
handle_win(const struct Minecurses *m)
{
    endscreen(m, GAME_WON);
    getchar();
    util_session_write(m, GAME_WON);
    util_score_write(m);
}

#undef bx
#undef by

void
navigate(struct Minecurses *m, int *mv)
{
    static int y = 1, x = 2;
    curs_update(m, y, x);
    m->x = ARRSPACE_X(x);
    m->y = ARRSPACE_Y(y);
    refresh();
    getmv(m, mv, &y, &x);
}

void
getmv(const struct Minecurses *m, int *mv, int *y, int *x)
{
    *mv = wgetch(m->gamewin);
    switch (*mv)
    {
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
curs_update(const struct Minecurses *m, int y, int x)
{
    wmove(m->gamewin, y, x);
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
endscreen(const struct Minecurses *m, State state)
{
    wclear(m->gamewin);
    wrefresh(m->gamewin);
    attron(A_BOLD);
    switch (state)
    {
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
