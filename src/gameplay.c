#include "gameplay.h"
#include "main.h"

#define X b->x
#define Y b->y

void
play(WINDOW *gw, Board *b)
{
	b->x = b->y = 0;
	b->gameover = FALSE;
	b->ndefused = 0;
	int move;

	do
	{
		erase();
		delwin(gw);
		refresh();
		gw = game_win(b->rows, b->cols);
		print_board(gw, b);
		session_info(b);
		navigate(gw, &move, b);
		switch (move)
		{
			case ENTER:			/* FALLTHROUGH */
			case OPEN_LOWER:
			case OPEN_UPPER:
				b->gameover = open_cell(gw, b);
				break;
			case FLAG_LOWER:	/* FALLTHROUGH */
			case FLAG_UPPER:
				handle_flags(gw, b);
				break;
			case DEFUSE_LOWER:	/* FALLTHROUGH */
			case DEFUSE_UPPER:
				if (b->db[Y][X] == FLAG && b->mb[Y][X] == MINE)
				{
					b->ndefused++;
					defuse_mine(gw, b);
				}
				else if (b->db[Y][X] == FLAG && b->mb[Y][X] != MINE)
					b->gameover = TRUE;              
				break;
			case PAUSE_AUDIO:
				pause_audio();
				break;
			case VOLUME_UP:		/* FALLTHROUGH */
			case VOLUME_DOWN:
				volume(move);
				break;
			case 'm':
				handle_menu(gw, b);
				break;
			case 'r':
				clear_board(b);
				reset(b);
				init_game(gw, b);
				break;
			default: break;
		}
	} while (((Y >= 0 && Y < b->rows) &&
			(X >= 0 && X < b->cols))  &&
			b->ndefused < b->nmines && !b->gameover &&
			move != QUIT);	

	if (b->gameover)
		handle_gameover(gw, b);
	if (b->ndefused == b->nmines)
		handle_win(gw, b);
}

int
open_cell(WINDOW *gw, Board *b)
{
	transfer(b);
	reveal(gw, b);
	return (b->db[Y][X] == MINE) ? TRUE : FALSE;
}

void
handle_flags(WINDOW *gw, Board *b)
{
	if (b->db[Y][X] == FLAG)
		b->db[Y][X] = BLANK;
	else if (b->db[Y][X] != FLAG &&
			 b->db[Y][X] != BLANK)
		return;
	else b->db[Y][X] = FLAG;
	reveal(gw, b);
}

void
defuse_mine(WINDOW *gw, Board *b)
{
	refresh();
	b->db[Y][X] = b->mb[Y][X] = DEFUSED;
	reveal(gw, b);
}

void
transfer(Board *b)
{
	b->db[Y][X] = b->mb[Y][X];
}

void
reveal(WINDOW *gw, Board *b)
{
	int y = b->y + 1;
	int x = 3 * b->x + 2;
	mvwaddch(gw, y, x, b->db[Y][X]);
	wrefresh(gw);
}

int
is_defused(Board *b)
{
	return (b->db[Y][X] == DEFUSED) ? TRUE : FALSE;
}

void
handle_menu(WINDOW *gw, Board *b)
{
	options_menu();
	box(gw, 0, 0);
	print_board(gw, b);
}

void
handle_gameover(WINDOW *gw, Board *b)
{
	game_over(gw);
	getchar();
	erase();
	refresh();
	box(gw, 0, 0);
	print_board(gw, b);
	wrefresh(gw);
	session_write(b, GAME_LOST);
}

void
handle_win(WINDOW *gw, Board *b)
{
	game_won(gw);
	getchar();
	session_write(b, GAME_WON);
	score_write(b);
}

#undef Y
#undef X
