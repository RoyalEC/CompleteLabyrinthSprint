#ifndef LABYRINTH_HPP
#define LABYRINTH_HPP

#include <iostream>
#include <vector>
#include <string>
#include "player.hpp"
#include "inputHandler.hpp"
#include <nlohmann/json.hpp>

class labyrinthMap {
private:
    int width, height;
    std::vector<std::string> labyrinth;
    inputHandler handler;
    int startX = 0;
    int startY = 0;


public:
    // Constructors and destructor
    labyrinthMap();
    labyrinthMap(int width, int height);
    labyrinthMap(labyrinthMap&&) noexcept = default;
    labyrinthMap& operator=(labyrinthMap&&) noexcept = default;

    // Deleted copy operations
    labyrinthMap(const labyrinthMap&) = delete;
    labyrinthMap& operator=(const labyrinthMap&) = delete;

    ~labyrinthMap() = default;

    // Maze generation
    void generateLabyrinth();
    static const char WALL;

    // Game mechanics
    void setPlayerPosition(Player& player, Player::PlayerDirection direction);
    bool isValidMove(Player& player, Player::PlayerDirection direction);
    bool gameOver(const Player& player) const;
    void startGame(std::vector<Player>& players);
    bool isEnd(const std::vector<Player>& players) const;
    void findStartTile();  // Finds 'S' in the labyrinth and sets startX/startY
    bool isValidMove(int fromX, int fromY, Player::PlayerDirection dir) const;
    std::pair<int, int> getEndPosition() const;



    // Serialization and output
    std::string getWebSocketMessageForPlayer(int playerNumber);
    std::string serializeToJson() const;
    void printLabyrinth() const;

    // Accessors
    const std::vector<std::string>& getLabyrinth() const;
    int getWidth() const;
    int getHeight() const;
    int getStartX() const { return startX; }
    int getStartY() const { return startY; }


    // Optional: Direct data setting if needed
    void setLabyrinthData(std::vector<std::string>&& newLab, int newW, int newH) {
        labyrinth = std::move(newLab);
        width = newW;
        height = newH;
    }
};

#endif

