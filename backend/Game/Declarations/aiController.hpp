#ifndef AICONTROLLER_HPP
#define AICONTROLLER_HPP

#include <memory>
#include <atomic>
#include <functional>
#include "Difficulty.hpp"
#include "player.hpp"
#include "labyrinth.hpp"

class aiController
{
private:
    std::shared_ptr<Player> aiPlayer;
    labyrinthMap& map;
    Difficulty difficulty;
    std::atomic<bool> running = true;

public:
    aiController(std::shared_ptr<Player> ai, labyrinthMap& gameMap, Difficulty diff);

    void makeMove(); // Called to perform AI action
    Player::PlayerDirection chooseNextMove();

    // Simplified version that uses internal state
    void runAI(std::function<bool()> isGameOver);

    void stop();

private:
    Player::PlayerDirection randomMove();
    Player::PlayerDirection greedyMove();
    Player::PlayerDirection pathfindingMove(); // BFS or A* placeholder
    Player::PlayerDirection pathfindingMove(Player* player, Game* game);

};

#endif // AICONTROLLER_HPP

