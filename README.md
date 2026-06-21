# Tic Tac Toe AI (C++)

A Tic Tac Toe game implemented in modern C++ with multiple AI difficulty levels, including a Minimax-powered hard mode. Playable both in the **terminal** and through a graphical **SFML GUI**.

---

## Features

* 3 Difficulty Levels

  * Easy → Random moves
  * Medium → Basic strategy (win/block + randomness)
  * Hard → Minimax AI (optimal play)

* AI System

  * Smart move evaluation
  * Recursive Minimax with Alpha-Beta pruning
  * Layered scoring: prefers faster wins, slower losses, and central control

* Terminal UI

  * Clean board rendering
  * Colored output using ANSI escape codes

* Graphical UI (SFML)

  * Mouse-driven 3x3 board
  * Difficulty-selection menu (Easy / Medium / Hard)
  * Live scoreboard and win / lose / tie screen
  * Reuses the exact same game engine and AI as the terminal version

* Score Tracking

  * Player wins
  * AI wins
  * Ties

---

## Project Structure

```
.
├── main.cpp               # Terminal entry point
├── gui_main.cpp           # GUI (SFML) entry point
├── TicTacToe.h / .cpp     # Core game engine (+ public GUI driver API)
├── Player.h / .cpp        # Human input handling (terminal)
├── XOBot.h / .cpp         # AI logic (Easy / Medium / Hard)
├── GameGui.h / .cpp       # SFML graphical front-end
├── Utils.h                # Terminal utilities (colors)
├── CMakeLists.txt         # Build all targets
```

---

## Build & Run

> Requires a C++23 compiler with `<print>` support (GCC 14+ / MSYS2 UCRT64).

### Option A — CMake (builds every target)

```bash
cmake -S . -B build
cmake --build build
# Executables land in ./build : gamehub, gamehub-gui, test_runner, execution-time
```

### Option B — g++ directly

**Terminal version**

```bash
g++ main.cpp TicTacToe.cpp XOBot.cpp Player.cpp -o gamehub -std=c++23 -Wall -Wextra -lstdc++exp
./gamehub
```

**Graphical version (SFML)** — requires SFML 2.6 (`sudo apt install libsfml-dev` on Debian/Ubuntu, or `pacman -S mingw-w64-ucrt-x86_64-sfml` on MSYS2)

```bash
# Linux
g++ gui_main.cpp GameGui.cpp TicTacToe.cpp XOBot.cpp Player.cpp \
    -o gamehub-gui -std=c++23 -Wall -Wextra -lstdc++exp \
    -lsfml-graphics -lsfml-window -lsfml-system
./gamehub-gui

# Windows (MSYS2 / MinGW)
g++ gui_main.cpp GameGui.cpp TicTacToe.cpp XOBot.cpp Player.cpp \
    -o gamehub-gui.exe -std=c++23 -Wall -Wextra -lstdc++exp \
    $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)
```

> SFML link order matters: `graphics → window → system`.
> The GUI loads a TrueType font from common system locations (DejaVu / Liberation / Arial);
> to bundle your own, drop a `DejaVuSans.ttf` into an `assets/` folder next to the executable.

### How to play the GUI

Pick a difficulty, then click any empty cell to place your `X`. The computer
(`O`) responds automatically. After a game ends, choose **Play Again** (same
difficulty) or **Menu** (change difficulty).

---

## AI Overview

### Easy Mode

* Completely random moves
* No strategy

### Medium Mode

* Attempts to win if possible
* Blocks opponent
* Otherwise plays randomly

### Hard Mode (Minimax)

* Explores all possible board states (Minimax with Alpha-Beta pruning)
* Assigns a **layered terminal score** so each tier dominates the next:

  * AI wins     → `(10 - depth) * 100 + positionBonus`  (faster wins score higher)
  * Player wins → `(depth - 10) * 100 + positionBonus`  (slower losses score higher)
  * Tie         → `positionBonus`                        (broken by central control)
* `positionBonus` = Σ(AI cell weights) − Σ(player cell weights), where each cell's weight
  is the number of winning lines through it (center 4, corners 3, edges 2)
* Always selects the optimal move

---

## Example Gameplay

```
Player (X): 0  |  Computer (O): 0 | Ties: 0
==== XO XO XO====
  X  |  O  |     
_____|_____|_____
     |  X  |     
_____|_____|_____
  O  |     |     
==== XO XO XO====
Enter number (1-9) to place your 'X':
```

---

## Future Improvements

- [x] Alpha-Beta pruning (performance optimization)

- [x] GUI version using SFML
- [ ] Test function $1/3$
  - [ ] Easy mode
  - [ ] Medium mode
  - [x] Hard mode
- [x] Execution time measurement with `<chrono>` (HardMode `findBestMove`)
  - Standalone benchmark: `execution-time.cpp`.
  - Two `HardMode` bots (X vs O) play a full game; every `findBestMove()` call is timed with `std::chrono::steady_clock` (`startTime` / `endTime` / `elapsedTime` in nanoseconds).
  - The match is repeated 20 times — perfect-play vs perfect-play is deterministic, so the move sequence is identical across games and each move position gets 20 timing samples.
  - Reports per-move-position avg / min / max plus an overall average across all 180 calls.
  - Build: `g++ execution-time.cpp XOBot.cpp TicTacToe.cpp Player.cpp -o execution-time -std=c++23 -Wall -Wextra -lstdc++exp`

---

## Author

* Developed by: [Pattaradanai Amazutsumi]
* Discord: [haszan._]
* Email: [natoodxd@gmail.com] or [2511311215@tni.ac.th]
* GitHub: [@hanzanwow]
* Field: Computer & AI Engineering

---

## Project BackGround
Created during my **1st year, 2nd semester** as a Computer Engineer student,as a project submission for the CPE-113 course.
<br>
This project demostrates my understanding of basic AI algorithms and OOP.

---

## What I Learned
- Implementing Minimax Algorithm from scratch
- Object-Oriented Desing in Modern C++/23
- Game state management and AI decision-making
- Lambda function
- Smart pointers
- Basic API

---

## License

This project is released into the public domain under The Unlicense.  
See the LICENSE file for details.
