#include "tetris.h"
#include <string.h>

void board_clear(Game *g) {
    memset(g->board, 0, sizeof(g->board));
}

bool board_collides(const Game *g, const Piece *p) {
    int cells[4][2];
    piece_cells(p, cells);
    for (int i = 0; i < 4; i++) {
        int x = cells[i][0];
        int y = cells[i][1];
        if (x < 0 || x >= BOARD_W) return true;
        if (y >= BOARD_H + HIDDEN_ROWS) return true;
        if (y >= 0 && g->board[y][x] != CELL_EMPTY) return true;
    }
    return false;
}

void board_lock(Game *g) {
    int cells[4][2];
    piece_cells(&g->current, cells);
    for (int i = 0; i < 4; i++) {
        int x = cells[i][0];
        int y = cells[i][1];
        if (y >= 0 && y < BOARD_H + HIDDEN_ROWS)
            g->board[y][x] = g->current.type + 1;
    }
}

int board_clear_lines(Game *g) {
    int cleared = 0;
    int total = BOARD_H + HIDDEN_ROWS;

    for (int y = total - 1; y >= 0; ) {
        bool full = true;
        for (int x = 0; x < BOARD_W; x++)
            if (g->board[y][x] == CELL_EMPTY) { full = false; break; }

        if (full) {
            /* shift everything above down by one */
            for (int yy = y; yy > 0; yy--)
                memcpy(g->board[yy], g->board[yy - 1], sizeof(g->board[0]));
            memset(g->board[0], 0, sizeof(g->board[0]));
            cleared++;
            /* do not decrement y — re-check same row */
        } else {
            y--;
        }
    }
    return cleared;
}