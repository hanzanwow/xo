#ifndef GAMEGUI_H
#define GAMEGUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include "TicTacToe.h"

namespace XO
{
    //* Graphical (SFML) front-end for the Tic Tac Toe engine.
    //  Owns a TicTacToe instance and drives it event-driven through the public
    //  GUI API (placeMove / status / botMove / ...), reusing the existing AI.
    class GameGui
    {
    public:
        GameGui();   // create the window and load a font
        void run();  // blocking main loop until the window is closed

    private:
        // High-level screen / turn state.
        enum class UiState
        {
            Menu,        // choose difficulty
            PlayerTurn,  // waiting for the human to click a cell
            BotThinking, // human just moved; AI plays on the next frame
            GameOver     // match finished; show result + replay controls
        };

        // --- layout constants (pixels) ---
        static constexpr unsigned WIN_W = 600u;
        static constexpr unsigned WIN_H = 760u;
        static constexpr float BOARD_X = 60.f;  // top-left of the 3x3 board
        static constexpr float BOARD_Y = 220.f;
        static constexpr float CELL = 160.f;    // 3 * 160 = 480 px board
        static constexpr float BOARD_SIZE = 3.f * CELL;

        TicTacToe game;          // the reused engine
        sf::RenderWindow window;
        sf::Font font;
        bool fontLoaded = false;

        UiState state = UiState::Menu;
        char lastStatus = ' ';

        // --- event handling ---
        void handleEvent(const sf::Event &event);
        void onMouseClick(int x, int y);
        void onBoardClick(int x, int y);
        void onMenuClick(int x, int y);
        void onGameOverClick(int x, int y);
        int cellFromPixel(int x, int y) const; // 0..8, or -1 if outside the board

        // --- turn flow ---
        void startNewGame(Difficulty d);
        void advanceAfterMove(bool playerJustMoved);

        // --- rendering ---
        void render();
        void drawGrid();
        void drawMarks();
        void drawXAt(int cell);
        void drawOAt(int cell);
        void drawScoreboard();
        void drawTitle(const std::string &text);
        void drawMenu();
        void drawGameOverControls();

        // --- small UI helpers ---
        sf::Text makeText(const std::string &s, unsigned size, float x, float y,
                          sf::Color color = sf::Color::White) const;
        // Draw a labelled button; returns its bounds for hit-testing.
        sf::FloatRect drawButton(const std::string &label, float x, float y,
                                 float w, float h, sf::Color fill);

        // Button rectangles (kept in sync with the draw* methods so clicks and
        // rendering agree on the same geometry).
        static sf::FloatRect easyButton();
        static sf::FloatRect mediumButton();
        static sf::FloatRect hardButton();
        static sf::FloatRect playAgainButton();
        static sf::FloatRect menuButton();

        const char *modeName() const;
    };
}

#endif
