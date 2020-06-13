#include "settings.h"

int
set_cols(void)
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
set_rows(void)
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
set_nmines(int dimensions)
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
