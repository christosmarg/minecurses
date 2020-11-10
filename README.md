# minescurses

A minesweeper game using `ncurses`. The game was originally made as a
university assignment of mine, but I decided to take it a bit further afterwards.

## How to play

The objective is to simply find and defuse all the mines.  

* Enter number of columns
* Enter number of rows
* Enter number of mines
* Move with `w`/`s`/`a`/`d` or Vim keys (`k`/`j`/`h`/`l`)
* Open cell with `[ENTER]`, or `o`
* Flag cell with `f`
* Defuse mine (only if flagged) with `g`
* You win if you defuse all the mines
* You lose in case you open a mine or you try to defuse a flagged cell not containing a mine

Additional controls

* `m`: Open control menu
* `r`: Restart the game
* `q`: Quit

## To Do

* Add colors
* Fix wrong mine spawning bug
* Fix occasional flickering and cursor

## Execution

### UNIX-like operating systems

**Dependencies**
* `ncurses`

```shell
$ cd path/to/minecurses
$ make && make run
$ make clean # optional
```

You can install `minecurses` by running `sudo make install clean`.
The binary will be installed in `/usr/local/bin`.

### Windows

Doesn't work.

## Preview

### Start screen
![startmenu](https://user-images.githubusercontent.com/54286563/78460772-52a25e00-76cc-11ea-976b-10212e228ca0.png)
### Gameplay
![gameplay](https://user-images.githubusercontent.com/54286563/78460709-bc6e3800-76cb-11ea-92f4-ab58e141e9be.png)
