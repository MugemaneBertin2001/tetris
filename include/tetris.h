#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define BOARD_W     10
#define BOARD_H     20
#define HIDDEN_ROWS 2

#define CELL_EMPTY  0

/* Tetromino types */
typedef enum {
    TET_I, TET_O, TET_T, TET_S, TET_Z, TET_J, TET_L, TET_COUNT
} TetType;

/* A tetromino piece */
typedef struct {
    TetType type;
    int     rot;        /* 0..3 */
    int     x, y;       /* top-left of the 4x4 bounding box on the board */
} Piece;

/* Game state */
typedef struct {
    int   board[BOARD_H + HIDDEN_ROWS][BOARD_W];
    Piece current;
    Piece next;
    TetType hold;
    bool  hold_used;

    int   score;
    int   lines;
    int   level;

    int   drop_interval_ms; /* gravity speed */
    bool  game_over;
    bool  paused;
} Game;

/* piece.c */
void  piece_spawn(Piece *p, TetType type);
void  piece_rotate(Piece *p, int dir);
bool  piece_cells(const Piece *p, int cells[4][2]);

/* board.c */
void  board_clear(Game *g);
bool  board_collides(const Game *g, const Piece *p);
void  board_lock(Game *g);
int   board_clear_lines(Game *g);

/* render.c */
void  render_init(void);
void  render_shutdown(void);
void  render(const Game *g);
void  render_game_over(const Game *g);

/* main.c helpers */
void  game_init(Game *g);
void  game_update(Game *g, int elapsed_ms);
TetType piece_random(void);

#endif