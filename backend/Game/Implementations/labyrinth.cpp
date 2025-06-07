#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <tuple>
#include <random>
#include "../Declarations/labyrinth.hpp"
#include "../Declarations/player.hpp"
#include "../Declarations/inputHandler.hpp"
#include "../Declarations/game.hpp"

const char labyrinthMap::WALL = '#';

// Default constructor
labyrinthMap::labyrinthMap() : width(0), height(0) {}

labyrinthMap::labyrinthMap(int w, int h)
    : width((w % 2 == 0) ? w + 1 : w), height((h % 2 == 0) ? h + 1 : h)
{
    std::cout << "📦 Constructing labyrinthMap with w=" << width << ", h=" << height << "\n";
    // DO NOT call generateLabyrinth() here unless you're certain it's safe cross-platform
}

void labyrinthMap::generateLabyrinth() {
    std::cout << "🧪 generateLabyrinth() called with width=" << width << ", height=" << height << "\n";

    if (width <= 0 || height <= 0) {
        std::cerr << "❌ Invalid dimensions! Maze not generated.\n";
        return;
    }

    labyrinth = std::vector<std::string>(height, std::string(width, WALL));

    std::stack<std::pair<int, int>> stack;
    int startX = 0, startY = 0;
    stack.push({ startX, startY });
    labyrinth[startY][startX] = ' ';

    std::vector<std::pair<int, int>> directions = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };
    std::default_random_engine rng(std::random_device{}());

    while (!stack.empty()) {
        auto [x, y] = stack.top();
        stack.pop();
        std::shuffle(directions.begin(), directions.end(), rng);

        for (auto [dx, dy] : directions) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && labyrinth[ny][nx] == WALL) {
                labyrinth[ny][nx] = ' ';
                labyrinth[y + dy / 2][x + dx / 2] = ' ';
                stack.push({ nx, ny });
            }
        }
    }

    labyrinth[0][0] = 'S';
    labyrinth[height / 2][width - 1] = 'E';

    findStartTile();

    std::cout << "✅ Labyrinth generation complete with " << labyrinth.size() << " rows.\n";
}

// Print for debugging
void labyrinthMap::printLabyrinth() const {
    for (const auto& row : labyrinth) {
        std::cout << row << std::endl;
    }
}

// Return full JSON string of the map
std::string labyrinthMap::getWebSocketMessageForPlayer(int playerNumber) {
    nlohmann::json message;
    message["labyrinth"] = labyrinth;
    message["width"] = width;
    message["height"] = height;
    return message.dump();
}

std::string labyrinthMap::serializeToJson() const {
    nlohmann::json j;
    j["labyrinth"] = labyrinth;
    j["width"] = width;
    j["height"] = height;
    return j.dump();
}

// Movement logic
void labyrinthMap::setPlayerPosition(Player& player, Player::PlayerDirection direction) {
    if (isValidMove(player, direction)) {
        switch (direction) {
        case Player::PlayerDirection::MoveUp:    player.setY(player.getY() - 1); break;
        case Player::PlayerDirection::MoveDown:  player.setY(player.getY() + 1); break;
        case Player::PlayerDirection::MoveLeft:  player.setX(player.getX() - 1); break;
        case Player::PlayerDirection::MoveRight: player.setX(player.getX() + 1); break;
        }
    }
}

bool labyrinthMap::isValidMove(Player& player, Player::PlayerDirection direction) {
    int newX = player.getX();
    int newY = player.getY();

    switch (direction) {
    case Player::PlayerDirection::MoveUp:    newY--; break;
    case Player::PlayerDirection::MoveDown:  newY++; break;
    case Player::PlayerDirection::MoveLeft:  newX--; break;
    case Player::PlayerDirection::MoveRight: newX++; break;
    }

    return newX >= 0 && newX < width && newY >= 0 && newY < height && labyrinth[newY][newX] != WALL;
}

bool labyrinthMap::gameOver(const Player& player) const {
    return labyrinth[player.getY()][player.getX()] == 'E';
}

// Called when game begins
void labyrinthMap::startGame(std::vector<Player>& players) {
    for (auto& player : players) {
        player.setX(0);
        player.setY(0);
    }
}

bool labyrinthMap::isEnd(const std::vector<Player>& players) const {
    for (const auto& player : players) {
        if (!gameOver(player)) return false;
    }
    return true;
}

// Accessors
const std::vector<std::string>& labyrinthMap::getLabyrinth() const {
    return labyrinth;
}

int labyrinthMap::getWidth() const {
    return width;
}

int labyrinthMap::getHeight() const {
    return height;
}

void labyrinthMap::findStartTile() {
    for (int y = 0; y < labyrinth.size(); ++y) {
        for (int x = 0; x < labyrinth[y].size(); ++x) {
            if (labyrinth[y][x] == 'S') {
                startX = x;
                startY = y;
                std::cout << "✅ Start tile found at (" << startX << ", " << startY << ")\n";
                return;
            }
        }
    }
    std::cerr << "⚠️ No start tile 'S' found in labyrinth!\n";
}

bool labyrinthMap::isValidMove(int fromX, int fromY, Player::PlayerDirection dir) const {
    int newX = fromX;
    int newY = fromY;

    switch (dir) {
    case Player::PlayerDirection::MoveUp:    newY--; break;
    case Player::PlayerDirection::MoveDown:  newY++; break;
    case Player::PlayerDirection::MoveLeft:  newX--; break;
    case Player::PlayerDirection::MoveRight: newX++; break;
    default: return false;
    }

    // Optionally check bounds first
    if (newX < 0 || newX >= getWidth() || newY < 0 || newY >= getHeight())
        return false;

    return labyrinth[newY][newX] != WALL;

}

std::pair<int, int> labyrinthMap::getEndPosition() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (labyrinth[y][x] == 'E') {
                return { x, y };
            }
        }
    }
    std::cerr << "⚠️ No 'E' tile found. Using fallback (width-1, height-1).\n";
    return { width - 1, height - 1 }; // fallback
}


