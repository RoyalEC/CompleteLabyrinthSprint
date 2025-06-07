#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include <memory> // For std::unique_ptr
#include <string>
#include <utility> // For std::pair
#include <nlohmann/json.hpp>
#include "inputHandler.hpp"

class Player {
private:
    int Id;
    char character; // 'P' for player
    int x, y;
    std::unique_ptr<inputHandler> inputHandler;
    int mapSize;

public:
    // Enum for player direction
    enum class PlayerDirection {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown
    };

    // Constructors and destructor
    Player() = default;
    Player(int Id, char character, int x, int y);

    ~Player() = default;
    std::pair<int, int> getPosition() const;


    // Deleted copy constructor and copy assignment operator
    Player(const Player& other) = delete;
    Player& operator=(const Player&) = delete;

    // Move constructor and move assignment operator
    Player(Player&&) noexcept;
    Player& operator=(Player&&) noexcept;

    // Movement and update functions
    void move(PlayerDirection direction);
    void setPosition(int x, int y);
    void update(std::string message);

    // Direction conversion
    static PlayerDirection stringToDirection(const std::string& actionInput);
    std::pair<int, int> actionToDelta(const std::string& action);

    // Getters and Setters
    int getX() const;
    int getY() const;
    void setX(int newX);
    void setY(int newY);
    int getId() const;
    void setMapSize(int size);
};

#endif // PLAYER_HPP


