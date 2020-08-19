#include "util.h"

void
util_reset(Minecurses *m)
{
    echo();
    m->cols       = util_cols_set();
    m->rows       = util_rows_set();
    m->nummines   = util_nummines_set(m->rows * m->cols);
    m->numdefused = m->x = m->y = 0;
    m->gameover   = FALSE;
    noecho();
    wins_menu_options();
    erase();
    refresh();
}

void
util_game_init(Minecurses *m)
{
    dispboard_init(m);
    mineboard_init(m);
}

int
util_cols_set(void)
{
    int cols;
    do
    {
        mvprintw(YMAX(stdscr)-4, 1, "Columns (Min = 5, Max = %d): ",
                ARRSPACE_X(XMAX(stdscr)) - 2);
        refresh();
        scanw("%d", &cols);
        refresh();
    } while (cols < 5 || cols > ARRSPACE_X(XMAX(stdscr)) - 2);
    return cols;    
}

int
util_rows_set(void)
{
    int rows;
    do
    {
        mvprintw(YMAX(stdscr)-3, 1, "Rows (Min = 5, Max = %d): ",
                ARRSPACE_Y(YMAX(stdscr)) - 3);
        refresh();
        scanw("%d", &rows);
        refresh();
    } while (rows < 5 || rows > ARRSPACE_Y(YMAX(stdscr)) - 3); 
    return rows;
}

int
util_nummines_set(int dimensions)
{
    int nmines;
    do
    {
        mvprintw(YMAX(stdscr)-2, 1, "Mines (Max = %d): ", dimensions-15);
        refresh();
        scanw("%d", &nmines);
        refresh();
    } while (nmines < 1 || nmines > dimensions-15);
    return nmines;
}

void
util_dealloc_boards(Minecurses *m)
{
    int i;
    for (i = 0; i < m->rows; i++)
    {
        free(m->dispboard[i]);
        free(m->mineboard[i]);
    }
    free(m->dispboard);
    free(m->mineboard);
}

void
util_session_write(const Minecurses *m, State state)
{
    int i, j;
    FILE *fsession = fopen(SESSION_PATH, "w");
    if (fsession == NULL) util_die();
    else
    {
        state == GAME_WON
            ? fprintf(fsession, "Mine hit at position (%d, %d)\n\n",
                    m->x+1, m->y+1)
            : fprintf(fsession, "Last mine defused at position (%d, %d)\n\n",
                    m->x+1, m->y+1);
        fprintf(fsession, "Board overview\n\n");
        for (i = 0; i < m->rows; i++)
        {
            for (j = 0; j < m->cols; j++)
                fprintf(fsession, "%c ", m->mineboard[i][j]);
            fprintf(fsession, "\n");
        }           
    }
    fclose(fsession);
}

void
util_score_write(const Minecurses *m)
{
    FILE *scorelog = fopen(SCORE_LOG_PATH, "a");
    char *playername = util_playername_get();
    if (scorelog == NULL) util_die();
    else
    {
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
util_playername_get(void)
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

void
util_die(void)
{
    mvprintw(0, 0, "Error, cannot allocate memory, exiting...");
    refresh();
    exit(EXIT_FAILURE);
}

