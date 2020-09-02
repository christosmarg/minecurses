#ifndef BOARDS_H 
#define BOARDS_H

#include <time.h>
#include "main.h"
#include "util.h"

#define BLANK ' '
#define MINE  '*'

#define BOARD_ALLOC(board, rows, cols)  do {            \
    size_t i;                                           \
    (board) = (char **)malloc((rows) * sizeof(char *)); \
    for (i = 0; i < (rows); i++)                        \
        (board)[i] = (char *)malloc((cols));            \
    if ((board) == NULL) util_die();                    \
} while (0)

extern void    dispboard_init(struct Minecurses *);
extern void    mineboard_init(struct Minecurses *);

#endif /* BOARDS_H */
