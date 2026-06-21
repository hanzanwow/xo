#include "GameGui.h"
#include <array>
#include <filesystem>
#include <iostream>

namespace
{
    // Palette echoing the terminal version's colors (Utils.h).
    const sf::Color COLOR_BG(28, 28, 36);
    const sf::Color COLOR_GRID(120, 120, 140);
    const sf::Color COLOR_X(80, 200, 120);    // green  (player)
    const sf::Color COLOR_O(200, 90, 200);    // magenta(computer)
    const sf::Color COLOR_TEXT(235, 235, 235);
    const sf::Color COLOR_EASY(70, 130, 180);
    const sf::Color COLOR_MEDIUM(180, 150, 60);
    const sf::Color COLOR_HARD(180, 70, 70);
    const sf::Color COLOR_BTN(80, 80, 110);
}

XO::GameGui::GameGui()
    : window(sf::VideoMode(WIN_W, WIN_H), "Tic Tac Toe - XO", sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);

    // Try a series of fonts so the binary works on Linux, MSYS2/Windows and macOS.
    static const std::array<const char *, 5> candidates = {
        "assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"};

    for (const auto *path : candidates)
    {
        // Skip missing paths first so SFML doesn't log a scary error for every
        // fallback candidate that simply isn't installed on this OS.
        std::error_code ec;
        if (!std::filesystem::exists(path, ec))
            continue;

        if (font.loadFromFile(path))
        {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded)
        std::cerr << "Warning: no font found; text labels will not be drawn.\n";
}

void XO::GameGui::run()
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            handleEvent(event);

        // The bot plays one ply on the frame after the human moves, so the
        // player's X is visible before the AI responds.
        if (state == UiState::BotThinking)
        {
            game.botMove();
            advanceAfterMove(false);
        }

        render();
    }
}

// ===================== Event handling =====================

void XO::GameGui::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::Closed)
    {
        window.close();
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        onMouseClick(event.mouseButton.x, event.mouseButton.y);
    }
}

void XO::GameGui::onMouseClick(int x, int y)
{
    switch (state)
    {
    case UiState::Menu:
        onMenuClick(x, y);
        break;
    case UiState::PlayerTurn:
        onBoardClick(x, y);
        break;
    case UiState::GameOver:
        onGameOverClick(x, y);
        break;
    case UiState::BotThinking:
        break; // ignore clicks while the AI is to move
    }
}

void XO::GameGui::onMenuClick(int x, int y)
{
    const sf::Vector2f p(static_cast<float>(x), static_cast<float>(y));
    if (easyButton().contains(p))
        startNewGame(Difficulty::Easy);
    else if (mediumButton().contains(p))
        startNewGame(Difficulty::Medium);
    else if (hardButton().contains(p))
        startNewGame(Difficulty::Hard);
}

void XO::GameGui::onBoardClick(int x, int y)
{
    const int cell = cellFromPixel(x, y);
    if (cell < 0)
        return;

    // placeMove rejects occupied / out-of-range cells, so this also guards clicks
    // on filled squares.
    if (game.placeMove(static_cast<size_t>(cell), game.getPlayerIcon()))
        advanceAfterMove(true);
}

void XO::GameGui::onGameOverClick(int x, int y)
{
    const sf::Vector2f p(static_cast<float>(x), static_cast<float>(y));
    if (playAgainButton().contains(p))
        startNewGame(game.getMode()); // same difficulty
    else if (menuButton().contains(p))
        state = UiState::Menu;
}

int XO::GameGui::cellFromPixel(int x, int y) const
{
    const float lx = static_cast<float>(x) - BOARD_X;
    const float ly = static_cast<float>(y) - BOARD_Y;
    if (lx < 0.f || ly < 0.f || lx >= BOARD_SIZE || ly >= BOARD_SIZE)
        return -1;

    const int col = static_cast<int>(lx / CELL);
    const int row = static_cast<int>(ly / CELL);
    return row * 3 + col; // matches the engine's 0..8 row-major indexing
}

// ===================== Turn flow =====================

void XO::GameGui::startNewGame(Difficulty d)
{
    game.setMode(d);
    game.newGame();
    lastStatus = ' ';
    state = UiState::PlayerTurn;
}

void XO::GameGui::advanceAfterMove(bool playerJustMoved)
{
    lastStatus = game.status();

    if (lastStatus != ' ')
    {
        game.recordResult(lastStatus);
        state = UiState::GameOver;
        return;
    }

    // Game continues: after the human moves, let the AI play next frame;
    // after the AI moves, hand control back to the human.
    state = playerJustMoved ? UiState::BotThinking : UiState::PlayerTurn;
}

// ===================== Rendering =====================

void XO::GameGui::render()
{
    window.clear(COLOR_BG);

    drawScoreboard();

    switch (state)
    {
    case UiState::Menu:
        drawTitle("Choose Difficulty");
        drawMenu();
        break;

    case UiState::PlayerTurn:
    case UiState::BotThinking:
        drawTitle(state == UiState::BotThinking ? "Computer is thinking..." : "Your turn (X)");
        drawGrid();
        drawMarks();
        break;

    case UiState::GameOver:
    {
        std::string msg = "It's a tie!";
        if (lastStatus == game.getPlayerIcon())
            msg = "You win!";
        else if (lastStatus == game.getComputerIcon())
            msg = "You lose!";
        drawTitle(msg);
        drawGrid();
        drawMarks();
        drawGameOverControls();
        break;
    }
    }

    window.display();
}

void XO::GameGui::drawGrid()
{
    const float thickness = 6.f;

    // Two inner vertical and two inner horizontal lines.
    for (int i = 1; i <= 2; ++i)
    {
        sf::RectangleShape v(sf::Vector2f(thickness, BOARD_SIZE));
        v.setPosition(BOARD_X + i * CELL - thickness / 2.f, BOARD_Y);
        v.setFillColor(COLOR_GRID);
        window.draw(v);

        sf::RectangleShape h(sf::Vector2f(BOARD_SIZE, thickness));
        h.setPosition(BOARD_X, BOARD_Y + i * CELL - thickness / 2.f);
        h.setFillColor(COLOR_GRID);
        window.draw(h);
    }
}

void XO::GameGui::drawMarks()
{
    for (int i = 0; i < 9; ++i)
    {
        const char c = game.getPositionAt(static_cast<size_t>(i));
        if (c == game.getPlayerIcon())
            drawXAt(i);
        else if (c == game.getComputerIcon())
            drawOAt(i);
    }
}

void XO::GameGui::drawXAt(int cell)
{
    const int row = cell / 3;
    const int col = cell % 3;
    const float cx = BOARD_X + col * CELL + CELL / 2.f;
    const float cy = BOARD_Y + row * CELL + CELL / 2.f;
    const float len = CELL * 0.55f;
    const float thickness = 14.f;

    for (float angle : {45.f, -45.f})
    {
        sf::RectangleShape bar(sf::Vector2f(len, thickness));
        bar.setOrigin(len / 2.f, thickness / 2.f);
        bar.setPosition(cx, cy);
        bar.setRotation(angle);
        bar.setFillColor(COLOR_X);
        window.draw(bar);
    }
}

void XO::GameGui::drawOAt(int cell)
{
    const int row = cell / 3;
    const int col = cell % 3;
    const float cx = BOARD_X + col * CELL + CELL / 2.f;
    const float cy = BOARD_Y + row * CELL + CELL / 2.f;
    const float radius = CELL * 0.30f;
    const float thickness = 12.f;

    sf::CircleShape ring(radius);
    ring.setOrigin(radius, radius);
    ring.setPosition(cx, cy);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(thickness);
    ring.setOutlineColor(COLOR_O);
    ring.setPointCount(48);
    window.draw(ring);
}

void XO::GameGui::drawScoreboard()
{
    if (!fontLoaded)
        return;

    const std::string line =
        "You (X): " + std::to_string(game.getPlayerWins()) +
        "   Computer (O): " + std::to_string(game.getComputerWins()) +
        "   Ties: " + std::to_string(game.getTies());
    window.draw(makeText(line, 22, BOARD_X, 24.f, COLOR_TEXT));

    if (state != UiState::Menu)
        window.draw(makeText(std::string("Mode: ") + modeName(), 20, BOARD_X, 56.f, COLOR_TEXT));
}

void XO::GameGui::drawTitle(const std::string &text)
{
    if (!fontLoaded)
        return;
    window.draw(makeText(text, 34, BOARD_X, 120.f, COLOR_TEXT));
}

void XO::GameGui::drawMenu()
{
    const sf::FloatRect e = easyButton(), m = mediumButton(), h = hardButton();
    drawButton("Easy", e.left, e.top, e.width, e.height, COLOR_EASY);
    drawButton("Medium", m.left, m.top, m.width, m.height, COLOR_MEDIUM);
    drawButton("Hard", h.left, h.top, h.width, h.height, COLOR_HARD);
}

void XO::GameGui::drawGameOverControls()
{
    const sf::FloatRect pa = playAgainButton(), mb = menuButton();
    drawButton("Play Again", pa.left, pa.top, pa.width, pa.height, COLOR_EASY);
    drawButton("Menu", mb.left, mb.top, mb.width, mb.height, COLOR_BTN);
}

// ===================== UI helpers =====================

sf::Text XO::GameGui::makeText(const std::string &s, unsigned size, float x, float y,
                               sf::Color color) const
{
    sf::Text text;
    text.setFont(font);
    text.setString(s);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

sf::FloatRect XO::GameGui::drawButton(const std::string &label, float x, float y,
                                      float w, float h, sf::Color fill)
{
    sf::RectangleShape box(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(fill);
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(20, 20, 28));
    window.draw(box);

    if (fontLoaded)
    {
        sf::Text text = makeText(label, 24, 0.f, 0.f, COLOR_TEXT);
        const sf::FloatRect tb = text.getLocalBounds();
        text.setPosition(x + (w - tb.width) / 2.f - tb.left,
                         y + (h - tb.height) / 2.f - tb.top);
        window.draw(text);
    }

    return sf::FloatRect(x, y, w, h);
}

// Button geometry (single source of truth for draw + hit-test).
sf::FloatRect XO::GameGui::easyButton() { return {BOARD_X, 220.f, BOARD_SIZE, 90.f}; }
sf::FloatRect XO::GameGui::mediumButton() { return {BOARD_X, 330.f, BOARD_SIZE, 90.f}; }
sf::FloatRect XO::GameGui::hardButton() { return {BOARD_X, 440.f, BOARD_SIZE, 90.f}; }

sf::FloatRect XO::GameGui::playAgainButton()
{
    return {BOARD_X, BOARD_Y + BOARD_SIZE + 24.f, BOARD_SIZE * 0.55f, 60.f};
}
sf::FloatRect XO::GameGui::menuButton()
{
    const float w = BOARD_SIZE * 0.40f;
    return {BOARD_X + BOARD_SIZE - w, BOARD_Y + BOARD_SIZE + 24.f, w, 60.f};
}

const char *XO::GameGui::modeName() const
{
    switch (game.getMode())
    {
    case Difficulty::Easy:
        return "Easy";
    case Difficulty::Medium:
        return "Medium";
    case Difficulty::Hard:
        return "Hard";
    }
    return "?";
}
