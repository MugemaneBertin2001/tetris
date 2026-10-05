#include "tetris.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

TetType piece_random(void) {
    /* Standard 7-bag randomizer */
    static TetType bag[TET_COUNT];
    static int idx = TET_COUNT;

    if (idx >= TET_COUNT) {
        for (int i = 0; i < TET_COUNT; i++) bag[i] = i;
        for (int i = TET_COUNT - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            TetType t = bag[i]; bag[i] = bag[j]; bag[j] = t;
        }
        idx = 0;
    }
    return bag[idx++];
}

static void spawn_next(Game *g) {
    g->current = g->next;
    piece_spawn(&g->current, g->current.type);
    g->next.type = piece_random();
    g->next.rot  = 0;
    g->hold_used = false;
    if (board_collides(g, &g->current))
        g->game_over = true;
}

void game_init(Game *g) {
    board_clear(g);
    g->score = 0;
    g->lines = 0;
    g->level = 1;
    g->drop_interval_ms = 800;
    g->game_over = false;
    g->paused = false;
    g->hold = TET_COUNT;
    g->hold_used = false;

    g->next.type = piece_random();
    g->next.rot  = 0;
    /* spawn dummy current then immediately replace with next */
    g->current.type = piece_random();
    piece_spawn(&g->current, g->current.type);
    spawn_next(g);
}

static void try_move(Game *g, int dx, int dy) {
    Piece p = g->current;
    p.x += dx;
    p.y += dy;
    if (!board_collides(g, &p))
        g->current = p;
}

static void try_rotate(Game *g, int dir) {
    Piece p = g->current;
    piece_rotate(&p, dir);

    /* simple wall kicks: try 0, then ±1, ±2 */
    const int kicks[] = {0, -1, 1, -2, 2};
    for (int i = 0; i < 5; i++) {
        Piece q = p;
        q.x += kicks[i];
        if (!board_collides(g, &q)) { g->current = q; return; }
    }
}

static void hard_drop(Game *g) {
    while (1) {
        Piece p = g->current;
        p.y++;
        if (board_collides(g, &p)) break;
        g->current = p;
    }
}

static void lock_and_next(Game *g) {
    board_lock(g);
    int n = board_clear_lines(g);

    if (n > 0) {
        static const int points[5] = { 0, 100, 300, 500, 800 };
        g->score += points[n] * g->level;
        g->lines += n;
        g->level = 1 + g->lines / 10;
        /* speed up: 800ms at L1 → min 100ms */
        g->drop_interval_ms = 800 - (g->level - 1) * 70;
        if (g->drop_interval_ms < 100) g->drop_interval_ms = 100;
    }
    spawn_next(g);
}

static void do_hold(Game *g) {
    if (g->hold_used) return;
    TetType saved = g->hold;
    g->hold = g->current.type;
    if (saved == TET_COUNT) {
        g->next.type = piece_random();
        spawn_next(g);
    } else {
        Piece p; piece_spawn(&p, saved);
        g->current = p;
    }
    g->hold_used = true;
}

void game_update(Game *g, int elapsed_ms) {
    static int acc = 0;
    if (g->game_over || g->paused) return;

    acc += elapsed_ms;
    if (acc >= g->drop_interval_ms) {
        acc = 0;
        Piece p = g->current;
        p.y++;
        if (board_collides(g, &p))
            lock_and_next(g);
        else
            g->current = p;
    }
}

static void handle_input(Game *g) {
    int ch = getch();
    while (ch != ERR) {
        if (g->game_over) return;
        switch (ch) {
        case 'q': case 'Q': g->game_over = true; return;
        case 'p': case 'P': g->paused = !g->paused; break;
        case KEY_LEFT:  if (!g->paused) try_move(g, -1, 0); break;
        case KEY_RIGHT: if (!g->paused) try_move(g,  1, 0); break;
        case KEY_DOWN:  if (!g->paused) {
                            try_move(g, 0, 1);
                            g->score += 1; /* soft-drop bonus */
                        } break;
        case KEY_UP: case 'x': case 'X':
                        if (!g->paused) try_rotate(g, 1); break;
        case 'z': case 'Z':
                        if (!g->paused) try_rotate(g, -1); break;
        case ' ':       if (!g->paused) { hard_drop(g); lock_and_next(g); } break;
        case 'c': case 'C':
                        if (!g->paused) do_hold(g); break;
        }
        ch = getch();
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    Game g;
    game_init(&g);

    render_init();

    struct timespec last, now;
    clock_gettime(CLOCK_MONOTONIC, &last);

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        int elapsed = (int)((now.tv_sec - last.tv_sec) * 1000 +
                            (now.tv_nsec - last.tv_nsec) / 1000000);
        last = now;

        handle_input(&g);

        if (g.game_over) {
            render_game_over(&g);
            /* wait for R or Q */
            int ch;
            do { ch = getch(); napms(50); } while (ch == ERR || (ch != 'r' && ch != 'R' && ch != 'q' && ch != 'Q'));
            if (ch == 'q' || ch == 'Q') break;
            game_init(&g);
            continue;
        }

        game_update(&g, elapsed);
        render(&g);
        napms(16); /* ~60 FPS */
    }

    render_shutdown();
    return 0;
}