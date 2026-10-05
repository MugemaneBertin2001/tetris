#include "tetris.h"
#include <ncurses.h>
#include <string.h>

static const int PIECE_COLOR[TET_COUNT] = {
    1, 2, 3, 4, 5, 6, 7
};

static const char *NAMES[TET_COUNT] = { "I","O","T","S","Z","J","L" };

void render_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();
    use_default_colors();

    init_pair(1, COLOR_CYAN,   -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_MAGENTA,-1);
    init_pair(4, COLOR_GREEN,  -1);
    init_pair(5, COLOR_RED,    -1);
    init_pair(6, COLOR_BLUE,   -1);
    init_pair(7, COLOR_WHITE,  -1);
    init_pair(8, COLOR_WHITE,  -1);  /* border / empty */
    init_pair(9, COLOR_BLACK,  COLOR_WHITE); /* text */
}

void render_shutdown(void) {
    endwin();
}

/* Draw a single cell at screen row/col */
static void draw_cell(int sy, int sx, int type, bool ghost) {
    if (type == CELL_EMPTY) {
        mvaddch(sy, sx, ACS_CKBOARD);
        return;
    }
    int pair = PIECE_COLOR[type - 1];
    if (ghost) {
        attron(A_DIM);
        mvaddch(sy, sx, ACS_CKBOARD | COLOR_PAIR(pair));
        attroff(A_DIM);
    } else {
        attron(COLOR_PAIR(pair) | A_BOLD);
        mvaddch(sy, sx, ACS_CKBOARD);
        attroff(COLOR_PAIR(pair) | A_BOLD);
    }
}

static void draw_board(const Game *g) {
    int oy = 1, ox = 2;
    attron(COLOR_PAIR(8));
    for (int x = 0; x <= BOARD_W; x++) {
        mvaddch(oy - 1,        ox + x, ACS_HLINE);
        mvaddch(oy + BOARD_H,  ox + x, ACS_HLINE);
    }
    for (int y = 0; y <= BOARD_H; y++) {
        mvaddch(oy + y, ox - 1,       ACS_VLINE);
        mvaddch(oy + y, ox + BOARD_W, ACS_VLINE);
    }
    mvaddch(oy - 1,        ox - 1,       ACS_ULCORNER);
    mvaddch(oy - 1,        ox + BOARD_W, ACS_URCORNER);
    mvaddch(oy + BOARD_H,  ox - 1,       ACS_LLCORNER);
    mvaddch(oy + BOARD_H,  ox + BOARD_W, ACS_LRCORNER);
    attroff(COLOR_PAIR(8));

    /* draw settled blocks (skip hidden rows) */
    for (int y = HIDDEN_ROWS; y < BOARD_H + HIDDEN_ROWS; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            int t = g->board[y][x];
            if (t) draw_cell(oy + (y - HIDDEN_ROWS), ox + x, t, false);
        }
    }

    /* ghost piece — where current would land */
    Piece ghost = g->current;
    while (!board_collides(g, &ghost)) ghost.y++;
    ghost.y--;
    int gc[4][2];
    piece_cells(&ghost, gc);
    for (int i = 0; i < 4; i++) {
        int y = gc[i][1] - HIDDEN_ROWS;
        int x = gc[i][0];
        if (y >= 0 && y < BOARD_H && x >= 0 && x < BOARD_W)
            draw_cell(oy + y, ox + x, g->current.type + 1, true);
    }

    /* active piece */
    int cells[4][2];
    piece_cells(&g->current, cells);
    for (int i = 0; i < 4; i++) {
        int y = cells[i][1] - HIDDEN_ROWS;
        int x = cells[i][0];
        if (y >= 0 && y < BOARD_H && x >= 0 && x < BOARD_W)
            draw_cell(oy + y, ox + x, g->current.type + 1, false);
    }
}

static void draw_panel(const Game *g) {
    int px = 2 + BOARD_W + 4;
    int py = 1;
    attron(COLOR_PAIR(9));
    mvprintw(py + 0, px, " TETRIS ");
    attroff(COLOR_PAIR(9));

    mvprintw(py + 2, px, "Score: %d", g->score);
    mvprintw(py + 3, px, "Lines: %d", g->lines);
    mvprintw(py + 4, px, "Level: %d", g->level);

    mvprintw(py + 6, px, "Next: %s", NAMES[g->next.type]);
    if (g->hold != TET_COUNT)
        mvprintw(py + 7, px, "Hold: %s", NAMES[g->hold]);

    mvprintw(py + 10, px, "←/→  move");
    mvprintw(py + 11, px, "↑/X  rotate CW");
    mvprintw(py + 12, px, "Z    rotate CCW");
    mvprintw(py + 13, px, "↓    soft drop");
    mvprintw(py + 14, px, "Spc  hard drop");
    mvprintw(py + 15, px, "C    hold");
    mvprintw(py + 16, px, "P    pause");
    mvprintw(py + 17, px, "Q    quit");
}

void render(const Game *g) {
    erase();
    draw_board(g);
    draw_panel(g);
    if (g->paused) {
        attron(A_BOLD | COLOR_PAIR(9));
        mvprintw(BOARD_H / 2, 3, "  PAUSED  ");
        attroff(A_BOLD | COLOR_PAIR(9));
    }
    refresh();
}

void render_game_over(const Game *g) {
    erase();
    int row = 4;
    attron(A_BOLD);
    mvprintw(row++, 4, "GAME OVER");
    attroff(A_BOLD);
    mvprintw(row++, 4, "Score: %d", g->score);
    mvprintw(row++, 4, "Lines: %d", g->lines);
    mvprintw(row++, 4, "Level: %d", g->level);
    mvprintw(row++, 4, "Press R to restart, Q to quit");
    refresh();
}