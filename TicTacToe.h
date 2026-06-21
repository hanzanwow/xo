#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <memory>
#include <array>
#include "XOBot.h"
#include "Player.h"

namespace XO
{
    class XOBot;
    class player;
    // game difficulty levels
    enum class Difficulty
    {
        Easy,
        Medium,
        Hard
    };

    class TicTacToe
    {
    private:
        // Smart pointers for Player and Bot management
        // instance of "new & delete"
        std::unique_ptr<player> ptrPlayer;
        std::unique_ptr<XOBot> ptrBot;

        Difficulty mode;
        std::array<char, 9> spaces = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
        const char playerIcon = 'X';
        const char computerIcon = 'O';
        bool running = true;

        int PlayerWins = 0;
        int ComputerWins = 0;
        int Ties = 0;

        Difficulty random_model();
        void drawBoard();
        void resetBoard();
        char checkGameStatus() const;
        bool PlayAgain();
        void updateScore(char status);
        void displayScore();
        void ShowStatus(char status);

    public:
        /*
        #########==================================================#########
        #########-----Application Programming Interface (API)------#########
        #########==================================================#########
            API for Player and Bot to interact with the game board
        */

        // place a move
        bool placeMove(size_t index, char icon)
        {
            if (index < 9uz && spaces[index] == ' ')
            {
                spaces.at(index) = icon;
                return true;
            }

            return false;
        }

        // Getters game icons
        char getPlayerIcon() const { return playerIcon; }
        char getComputerIcon() const { return computerIcon; }

        // Returns character at board position
        char getPositionAt(size_t index) const { return spaces.at(index); }

        // Return board array
        auto &getMap() const { return spaces; }

        // Returns game mode
        auto &getMode() const { return mode; }

        /*
        #########==================================================#########
        #########----- GUI / external-driver API (additive) ------#########
        #########==================================================#########
            Thin public wrappers that let an event-driven front-end (e.g. the
            SFML GUI) reuse the engine without the terminal-coupled runGame()
            loop. None of these touch std::cin / std::cout.
        */

        // Reset the board and (re)start a match. Does NOT pick a mode or print.
        void newGame()
        {
            resetBoard();
            running = true;
        }

        // Public, read-only wrapper over checkGameStatus().
        // Returns 'X' | 'O' | 'T' | ' ' (space = still in progress).
        char status() const { return checkGameStatus(); }

        // Drive one AI move using the engine's own XOBot (Easy/Medium/Hard).
        void botMove() { ptrBot->Move(); }

        // Set the difficulty (pairs with getMode()).
        void setMode(Difficulty d) { mode = d; }

        // Apply the engine's score bookkeeping for a finished match.
        void recordResult(char finalStatus) { updateScore(finalStatus); }

        // Score getters for the GUI scoreboard.
        int getPlayerWins() const { return PlayerWins; }
        int getComputerWins() const { return ComputerWins; }
        int getTies() const { return Ties; }

        TicTacToe();
        ~TicTacToe();

        void runGame();
    };
}
#endif
