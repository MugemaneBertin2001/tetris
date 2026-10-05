# Tetris in C 🎮

A terminal-based Tetris clone written in C using `ncurses`.

![C](https://img.shields.io/badge/language-C11-blue.svg)
![ncurses](https://img.shields.io/badge/library-ncurses-green.svg)
![license](https://img.shields.io/badge/license-MIT-lightgrey.svg)

## Features

- **7-bag randomizer** — fair piece distribution, no droughts
- **Ghost piece** — preview where the current piece will land
- **Hold** — stash a piece once per spawn
- **Next-piece preview**
- **Wall kicks** — rotation feels forgiving near walls
- **Scoring** — 100 / 300 / 500 / 800 × level for 1–4 lines
- **Soft-drop bonus** — +1 point per cell
- **Levels** — every 10 lines, gravity speeds up
- **Colors + borders** — via `ncurses`

## Controls

| Key           | Action                        |
|---------------|-------------------------------|
| `←` / `→`     | Move left / right             |
| `↓`           | Soft drop (+1 point per cell) |
| `Space`       | Hard drop                     |
| `↑` or `X`    | Rotate clockwise              |
| `Z`           | Rotate counter-clockwise      |
| `C`           | Hold piece                    |
| `P`           | Pause                         |
| `Q`           | Quit                          |
| `R`           | Restart (after game over)     |

## Requirements

- A C11 compiler (`gcc` or `clang`)
- `ncurses` development headers
- `make`

### Install dependencies

**macOS (Homebrew):**
```sh
brew install ncurses