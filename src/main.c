#include "audio.h"
#include "gameplay.h"
#include "main.h"
#include "minesweeper.h"
#include "settings.h"
#include "wins.h"
#include <pthread.h>

int
main(int argc, char **argv)
{   
#ifndef NCURSES_VERSION
    fprintf(stderr, "ncurses is needed in order to run this program.\n");
    return EXIT_FAILURE;
#endif /* NCURSES_VERSION */
    curses_init();
    Board b;
    reset(&b);
    b.gw = game_win_init(b.rows, b.cols);
    game_init(&b);

    pthread_t audiothread;
    long tid = 1;
    pthread_create(&audiothread, NULL, audio_play, (void *)tid);
    minesweeper_play(&b);
    
    pthread_cancel(audiothread);
    dealloc_board(&b);
    delwin(b.gw);
    endwin();
    return EXIT_SUCCESS;
}

void
reset(Board *b)
{
    echo();
    b->cols     = cols_set();
    b->rows     = rows_set();
    b->nmines   = nmines_set(b->rows * b->cols);
    b->ndefused = b->x = b->y = 0;
    b->gameover = FALSE;
    noecho();
    menu_options();
    erase();
    refresh();
}

void
game_init(Board *b)
{
    displayboard_init(b);
    mineboard_init(b);
}

void
dealloc_board(Board *b)
{
    int i;
    for (i = 0; i < b->rows; i++)
    {
        free(b->db[i]);
        free(b->mb[i]);
    }
    free(b->db);
    free(b->mb);
}

void
die(void)
{
    mvprintw(0, 0, "Error, cannot allocate memory, exiting...");
    refresh();
    exit(EXIT_FAILURE);
}
