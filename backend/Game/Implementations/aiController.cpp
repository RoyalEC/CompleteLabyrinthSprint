#include "../Declarations/aiController.hpp"
#include "../Declarations/Difficulty.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <random>
#include <atomic>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <limits>

// Helper to convert direction to string
std::string directionToString(Player::PlayerDirection dir) {
    switch (dir) {
    case Player::PlayerDirection::MoveUp: return "Up";
    case Player::PlayerDirection::MoveDown: return "Down";
    case Player::PlayerDirection::MoveLeft: return "Left";
    case Player::PlayerDirection::MoveRight: return "Right";
    default: return "Unknown";
    }
}

aiController::aiController(std::shared_ptr<Player> ai, labyrinthMap& gameMap, Difficulty diff)
    : aiPlayer(ai), map(gameMap), difficulty(diff)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void aiController::makeMove()
{
    if (!aiPlayer) return;

    Player::PlayerDirection dir;

    switch (difficulty) {
    case Difficulty::EASY:
        dir = randomMove();
        break;
    case Difficulty::MEDIUM:
        dir = greedyMove();
        break;
    case Difficulty::HARD:
        dir = pathfindingMove();
        break;
    }

    map.setPlayerPosition(*aiPlayer, dir);
    std::cout << "[AI] Moved " << directionToString(dir)
        << " to (" << aiPlayer->getX() << ", " << aiPlayer->getY() << ")\n";

    if (map.gameOver(*aiPlayer)) {
        std::cout << "[AI] Reached the goal!\n";
    }
}

Player::PlayerDirection aiController::randomMove()
{
    std::vector<Player::PlayerDirection> directions = {
        Player::PlayerDirection::MoveUp,
        Player::PlayerDirection::MoveDown,
        Player::PlayerDirection::MoveLeft,
        Player::PlayerDirection::MoveRight
    };
    std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device{}()));

    for (auto dir : directions) {
        if (map.isValidMove(*aiPlayer, dir))
            return dir;
    }

    return Player::PlayerDirection::MoveUp; // fallback
}

Player::PlayerDirection aiController::greedyMove()
{
    int x = aiPlayer->getX();
    int y = aiPlayer->getY();
    int goalX = map.getWidth() - 1;
    int goalY = map.getHeight() - 1;

    std::vector<std::pair<Player::PlayerDirection, int>> options;

    std::vector<std::pair<Player::PlayerDirection, std::pair<int, int>>> candidates = {
        { Player::PlayerDirection::MoveUp,    { x, y - 1 } },
        { Player::PlayerDirection::MoveDown,  { x, y + 1 } },
        { Player::PlayerDirection::MoveLeft,  { x - 1, y } },
        { Player::PlayerDirection::MoveRight, { x + 1, y } }
    };

    for (auto& [dir, pos] : candidates) {
        int px = pos.first;
        int py = pos.second;

        // Temporarily update position for validation
        aiPlayer->setPosition(px, py);
        if (map.isValidMove(*aiPlayer, dir)) {
            int dist = std::abs(px - goalX) + std::abs(py - goalY);
            options.push_back({ dir, dist });
        }
        // Restore original position
        aiPlayer->setPosition(x, y);
    }

    if (!options.empty()) {
        std::sort(options.begin(), options.end(), [](auto& a, auto& b) {
            return a.second < b.second;
            });
        return options.front().first;
    }

    return Player::PlayerDirection::MoveUp;
}

Player::PlayerDirection aiController::pathfindingMove()
{
    auto [startX, startY] = aiPlayer->getPosition();
    auto [goalX, goalY] = map.getEndPosition();


    struct Node {
        int x, y, g, f;
        bool operator>(const Node& other) const { return f > other.f; }
    };

    auto toKey = [](int x, int y) { return (x << 16) | y; };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, std::pair<int, int>> cameFrom;
    std::unordered_map<int, int> gScore;

    openSet.push({ startX, startY, 0, std::abs(goalX - startX) + std::abs(goalY - startY) });
    gScore[toKey(startX, startY)] = 0;

    std::vector<std::pair<int, int>> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    while (!openSet.empty()) {
        Node current = openSet.top(); openSet.pop();

        if (current.x == goalX && current.y == goalY) {
            // Reconstruct the path
            int cx = goalX, cy = goalY;
            int px = cx, py = cy;
            while (cameFrom.count(toKey(cx, cy))) {
                std::tie(px, py) = cameFrom[toKey(cx, cy)];
                if (px == startX && py == startY) break;
                cx = px; cy = py;
            }

            int dx = cx - startX;
            int dy = cy - startY;

            if (dx == 0 && dy == -1) return Player::PlayerDirection::MoveUp;
            if (dx == 0 && dy == 1)  return Player::PlayerDirection::MoveDown;
            if (dx == -1 && dy == 0) return Player::PlayerDirection::MoveLeft;
            if (dx == 1 && dy == 0)  return Player::PlayerDirection::MoveRight;
        }

        for (const auto& dirPair : directions) {
            int dx = dirPair.first;
            int dy = dirPair.second;
            int nx = current.x + dx;
            int ny = current.y + dy;

            Player::PlayerDirection dir;
            if (dx == 0 && dy == -1) dir = Player::PlayerDirection::MoveUp;
            else if (dx == 0 && dy == 1)  dir = Player::PlayerDirection::MoveDown;
            else if (dx == -1 && dy == 0) dir = Player::PlayerDirection::MoveLeft;
            else if (dx == 1 && dy == 0)  dir = Player::PlayerDirection::MoveRight;
            else continue;

            if (!map.isValidMove(current.x, current.y, dir)) continue;

            int tentativeG = current.g + 1;
            int key = toKey(nx, ny);
            if (!gScore.count(key) || tentativeG < gScore[key]) {
                gScore[key] = tentativeG;
                cameFrom[key] = { current.x, current.y };
                int h = std::abs(goalX - nx) + std::abs(goalY - ny);
                openSet.push({ nx, ny, tentativeG, tentativeG + h });
            }
        }
    }

    return greedyMove(); // fallback
}




void aiController::runAI(std::function<bool()> isGameOver) {
    if (!aiPlayer) return;

    while (!isGameOver() && running.load()) {
        makeMove();

        if (map.gameOver(*aiPlayer)) {
            std::cout << "[AI] Reached the goal! Game over.\n";

            break; // stop the AI loop
        }

        int delayMs = (4 - difficulty) * 250;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}


void aiController::stop() {
    running.store(false);
}
