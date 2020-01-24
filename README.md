# Minesweeper

A simplistic minesweeper game using the `ncurses` library. The game was originally made as a university assignment of mine, but I decided to take it a bit further afterwards.

## How to play

* Enter number of columns
* Enter number of rows
* Enter number of mines
* Move with `w`/`s`/`a`/`d` or Vim keys (`k`/`j`/`h`/`l`)
* Open cell with `[ENTER]`, `[SPACE]`, or `o`
* Flag cell with `f`
* Defuse mine (only if flagged) with `g`
* You win if you defuse all the mines
* You lose in case you open a mine or you try to defuse a flagged cell not containing a mine

Additional controls

* `p`: Pause audio
* `+`: Volume up
* `-`: Volume down
* `q`: Quit

## To Do

* Handle resizing
* Add colors

## Execution

### Linux

**Dependencies**
* `ncurses`
* `SDL2`
* `SDL2_mixer`

```shell
$ cd full_path_to/minesweeper
$ make
$ make run
$ make clean # optional
```

### Windows (not tested)

**Dependencies:**   
* MinGW gcc compiler
* `make`
* `PDCurses`
* `SDL2`
* `SDL2_mixer`

In CMD, do the following:

```bat
cd full_path_to/minesweeper
make
make run
make clean
```
## Preview

![MinesweeperScr](https://user-images.githubusercontent.com/54286563/73043435-bf4be180-3e6d-11ea-9a72-9a6250ab1f64.png)
