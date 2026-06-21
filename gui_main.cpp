#include "GameGui.h"
#include <ctime>
#include <cstdlib>

// SFML graphical version of the game.
// Build (Linux):
//   g++ gui_main.cpp GameGui.cpp TicTacToe.cpp XOBot.cpp Player.cpp
//       -o gamehub-gui -std=c++23 -Wall -Wextra -lstdc++exp
//       -lsfml-graphics -lsfml-window -lsfml-system
// Run: ./gamehub-gui
int main()
{
    // Easy/Medium AI use std::rand, so seed it just like the terminal version.
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    XO::GameGui gui;
    gui.run();

    return 0;
}
