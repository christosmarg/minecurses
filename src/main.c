#include "gameplay.h"
#include "main.h"
#include "util.h"
#include <pthread.h>

static void
curses_init(void)
{
    initscr();
    noecho();
    cbreak();
}

int
main(int argc, char **argv)
{   
#ifndef NCURSES_VERSION
    fprintf(stderr, "ncurses is needed in order to run this program.\n");
    return EXIT_FAILURE;
#endif /* NCURSES_VERSION */
    curses_init();
    Minecurses m;
    util_reset(&m);
    m.gamewin = wins_gamewin_init(m.rows, m.cols);
    util_game_init(&m);

    pthread_t audiothread;
    long tid = 1;
    pthread_create(&audiothread, NULL, audio_play, (void *)tid);
    gmpl_start(&m);
    
    pthread_cancel(audiothread);
    util_dealloc_boards(&m);
    delwin(m.gamewin);
    endwin();
    return EXIT_SUCCESS;
}
