#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "../Declarations/inputHandler.hpp"
#include "../Declarations/player.hpp"

Player::Player(int Id, char character, int x, int y)
    : Id(Id), character(character), x(x), y(y) {
}

Player::Player(Player&& other) noexcept
    : Id(std::exchange(other.Id, 0)),
    character(std::exchange(other.character, '\0')),
    x(std::exchange(other.x, 0)),
    y(std::exchange(other.y, 0)),
    inputHandler(std::move(other.inputHandler)) {
}

Player& Player::operator=(Player&& other) noexcept
{
    if (this != &other)
    {
        Id = std::exchange(other.Id, 0);
        character = std::exchange(other.character, '\0');
        x = std::exchange(other.x, 0);
        y = std::exchange(other.y, 0);
        inputHandler = std::move(other.inputHandler);
    }
    return *this;
}

Player::PlayerDirection Player::stringToDirection(const std::string& actionInput)
{
    std::string action = actionInput;
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);

    if (action == "up" || action == "w" || action == "u")
        return Player::PlayerDirection::MoveUp;
    else if (action == "down" || action == "s" || action == "d")
        return Player::PlayerDirection::MoveDown;
    else if (action == "left" || action == "a" || action == "l")
        return Player::PlayerDirection::MoveLeft;
    else if (action == "right" || action == "r")
        return Player::PlayerDirection::MoveRight;
    else
        throw std::invalid_argument("Invalid action");
}

void Player::setMapSize(int size)
{
    mapSize = size;
}

void Player::move(Player::PlayerDirection direction)
{
    int newX = x;
    int newY = y;

    if (direction == PlayerDirection::MoveUp)
        newY -= 1;
    else if (direction == PlayerDirection::MoveDown)
        newY += 1;
    else if (direction == PlayerDirection::MoveLeft)
        newX -= 1;
    else if (direction == PlayerDirection::MoveRight)
        newX += 1;
    else {
        std::cerr << "Unknown direction" << std::endl;
        return;
    }

    setPosition(newX, newY);
}

void Player::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

int Player::getId() const
{
    return this->Id;
}

int Player::getX() const
{
    return this->x;
}

int Player::getY() const
{
    return this->y;
}

void Player::setX(int newX)
{
    x = newX;
}

void Player::setY(int newY)
{
    y = newY;
}

std::pair<int, int> Player::actionToDelta(const std::string& action)
{
    int deltaX = 0;
    int deltaY = 0;

    if (action == "up" || action == "w" || action == "u")
        deltaY = -1;
    else if (action == "down" || action == "s" || action == "d")
        deltaY = 1;
    else if (action == "left" || action == "a" || action == "l")
        deltaX = -1;
    else if (action == "right" || action == "r")
        deltaX = 1;

    return std::make_pair(deltaX, deltaY);
}

void Player::update(std::string message)
{
    auto [player, action] = this->inputHandler->handleWebSocketInput(message);
    auto [deltaX, deltaY] = this->actionToDelta(action);
    x += deltaX;
    y += deltaY;
    std::cout << "Player " << Id << " is at " << x << ", " << y << std::endl;
}

std::pair<int, int> Player::getPosition() const {
    return { x, y };
}

