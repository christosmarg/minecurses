#include "gameplay.h"

void play_minesweeper(WINDOW *gameWin, char **dispboard, char **mineboard, int COLS, int ROWS, int NMINES)
{
    int mbx = 0, mby = 0;
    bool gameOver = false, cantFlag = false;
    int numDefused = 0;
    int yMax, xMax, yMiddle, xMiddle;
    char move;
    getmaxyx(stdscr, yMax, xMax);
    yMiddle = yMax/2;
    xMiddle = xMax/2;

	print_board(gameWin, dispboard, COLS, ROWS);
    
    do
    {
        navigate(gameWin, dispboard, &move, &mbx, &mby);
        
		if (move == ENTER || move == OPEN_LOWER || move == OPEN_UPPER) // handle cell opening
        {
			cantFlag = true;
			open_cell(gameWin, dispboard, mineboard, mby, mbx, cantFlag, gameOver);
        }
        else if (move == FLAG_LOWER || move == FLAG_UPPER) // handle falgs
        {
			handle_flags(gameWin, dispboard, mineboard, mby, mbx);
        }
        else if (move == DEFUSE_LOWER || move == DEFUSE_UPPER) // check for defuse
        {
			if (dispboard[mby][mbx] == FLAG && mineboard[mby][mbx] == MINE)
            {
				numDefused++;
				defuse_mine(gameWin, dispboard, mineboard, mby, mbx);
            }
            else if (dispboard[mby][mbx] == FLAG && mineboard[mby][mbx] != MINE) gameOver = true;              
        }
        else if (move == PAUSE_AUDIO) pause_audio(); // handle audio
        else if (move == VOLUME_UP || move == VOLUME_DOWN) volume(move);

        mvprintw(1, xMiddle-8, "Defused mines: %d/%d", numDefused, NMINES);
        
    } while (((mby >= 0 && mby < ROWS) && (mbx >= 0 && mbx < COLS)) &&
             numDefused < NMINES && !gameOver && move != QUIT);
    
    if (gameOver == true)
    {
        game_over(gameWin, mineboard, yMiddle, xMiddle);
        getchar();
        print_board(gameWin, mineboard, COLS, ROWS);
        session_write(mineboard, COLS, ROWS, mbx, mby, "lost");
    }

    if (numDefused == NMINES)
    {
        game_won(gameWin, yMiddle, xMiddle);
        getchar();
        session_write(mineboard, COLS, ROWS, mbx, mby, "won");
        score_write(numDefused, COLS, ROWS);
    }
}

bool open_cell(WINDOW *gameWin, char **dispboard, char **mineboard, int mby, int mbx, bool gameOver)
{
	transfer(dispboard, mineboard, mby, mbx);
	reveal(gameWin, dispboard, mby, mbx, mby+1, 3*mbx+2);
	if (dispboard[mby][mbx] == MINE) gameOver = true;
	return gameOver;
}

void handle_flags(WINDOW *gameWin, char **dispboard, char **mineboard, int mby, int mbx)
{
	if (dispboard[mby][mbx] == FLAG) dispboard[mby][mbx] = BLANK; // undo flag 
	else if (dispboard[mby][mbx] != FLAG && dispboard[mby][mbx] != BLANK) return; // dont flag an already opened mine
	else dispboard[mby][mbx] = FLAG; // flag if not flagged already
	reveal(gameWin, dispboard, mby, mbx, mby+1, 3*mbx+2);
}

void defuse_mine(WINDOW *gameWin, char **dispboard, char **mineboard, int mby, int mbx)
{
	refresh();
	dispboard[mby][mbx] = mineboard[mby][mbx] = DEFUSED;
	reveal(gameWin, dispboard, mby, mbx, mby+1, 3*mbx+2);
}

void transfer(char **dispboard, char **mineboard, int mby, int mbx)
{
    dispboard[mby][mbx] = mineboard[mby][mbx];
}

void reveal(WINDOW *gameWin, char **dispboard, int mby, int mbx, int yLoc, int xLoc)
{
    mvwaddch(gameWin, yLoc, xLoc, dispboard[mby][mbx]);
    wrefresh(gameWin);
}

bool is_defused(char **dispboard, char **mineboard, int mby, int mbx)
{
    return ((dispboard[mby][mbx] == DEFUSED)) ? true : false;
}
