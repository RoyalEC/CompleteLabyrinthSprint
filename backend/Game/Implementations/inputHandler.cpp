#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "../Declarations/player.hpp"
#include "../Declarations/inputHandler.hpp"
#include "../Declarations/game.hpp"  // Required to manipulate game settings

using json = nlohmann::json;

inputHandler::inputHandler() {}
inputHandler::~inputHandler() {}
inputHandler::inputHandler(std::map<int, std::shared_ptr<Player>>& playerMap) : playerMap(playerMap) {}

inputHandler& inputHandler::operator=(inputHandler&& other) noexcept {
    if (this != &other) {
        this->playerMap = std::move(other.playerMap);
        this->game = other.game;
    }
    return *this;
}

void inputHandler::setGame(Game* gameInstance) {
    this->game = gameInstance;
}

std::shared_ptr<Player> inputHandler::getId(int playerId) {
    std::cout << "PlayerId: " << playerId << std::endl;
    auto it = playerMap.find(playerId);
    if (it != playerMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Player ID not found.");
}

void inputHandler::addPlayer(int playerId, std::shared_ptr<Player> player) {
    if (playerMap.find(playerId) == playerMap.end()) {
        playerMap[playerId] = player;
        std::cout << "Added player with ID " << playerId << " to inputHandler's playerMap." << std::endl;
    }
}

void inputHandler::updateInput(std::shared_ptr<Player> player, const std::string& action) {
    handleInput(player, action);
}

std::pair<std::shared_ptr<Player>, std::string> inputHandler::handleWebSocketInput(const std::string& message) {
    if (message.empty()) {
        std::cout << "Received empty message. Ignoring." << std::endl;
        return { nullptr, "" };
    }

    std::cout << "Received message: " << message << std::endl;
    auto json = nlohmann::json::parse(message);

    // Handle configuration messages
    if (json.contains("type") && json["type"] == "config") {
        if (!game) {
            std::cerr << "Game instance not set. Cannot configure." << std::endl;
            return { nullptr, "" };
        }

        std::string mode = json.value("mode", "single");
        std::string difficulty = json.value("difficulty", "easy");

        bool isSingle = (mode == "single" || mode == "singleplayer");
        game->setSinglePlayerMode(isSingle);
        game->setDifficulty(difficulty);

        std::cout << "Game configured via WebSocket: mode = " << mode << ", difficulty = " << difficulty << std::endl;
        return { nullptr, "" };
    }

    // --- Player movement ---
    if (playerMap.empty()) {
        std::cerr << "Player map is empty. Ignoring message." << std::endl;
        return { nullptr, "" };
    }

    std::shared_ptr<Player> player;

    if (json.contains("playerId") && json["playerId"].is_number_integer()) {
        int playerId = json["playerId"];
        auto it = playerMap.find(playerId);
        if (it != playerMap.end()) {
            player = it->second;
        }
        else {
            std::cerr << "Player ID " << playerId << " not found in inputHandler." << std::endl;
            return { nullptr, "" };
        }
    }
    else {
        std::cerr << "Missing or invalid 'playerId' in message. Ignoring." << std::endl;
        return { nullptr, "" };
    }

    if (!json.contains("action") || json["action"].is_null()) {
        throw std::runtime_error("Missing or null 'action' key in JSON message.");
    }

    std::string action = json["action"];
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);
    if (action.rfind("move", 0) == 0) {
        action = action.substr(4);
    }

    try {
        Player::PlayerDirection direction = Player::stringToDirection(action);
        return { player, action };
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid action: " << e.what() << std::endl;
        return { player, "" };
    }
}

void inputHandler::handleInput(std::shared_ptr<Player> player, const std::string& action) {
    if (!player || action.empty()) {
        std::cerr << "Invalid player or empty action received. Ignoring." << std::endl;
        return;
    }

    Player::PlayerDirection direction;
    if (action == "up") direction = Player::PlayerDirection::MoveUp;
    else if (action == "down") direction = Player::PlayerDirection::MoveDown;
    else if (action == "left") direction = Player::PlayerDirection::MoveLeft;
    else if (action == "right") direction = Player::PlayerDirection::MoveRight;
    else {
        std::cerr << "Unknown action: " << action << std::endl;
        return;
    }

    if (!game) {
        std::cerr << "❌ Game pointer is null in inputHandler. Cannot move player safely.\n";
        return;
    }

    int oldX = player->getX();
    int oldY = player->getY();

    // Attempt to move using labyrinth logic (checks for walls)
    game->getCurrentlevel().setPlayerPosition(*player, direction);

    int newX = player->getX();
    int newY = player->getY();

    if (oldX != newX || oldY != newY) {
        std::cout << "✅ Player " << player->getId() << " moved to (" << newX << ", " << newY << ")\n";

        if (game->getCurrentlevel().gameOver(*player)) {
            std::cout << "🎉 Player " << player->getId() << " reached the end! Game over.\n";
            // Optional: broadcast a game over message
            game->broadcastWinMessage(player->getId());
        }

    }
    else {
        std::cout << "⛔ Move blocked by wall at (" << newX << ", " << newY << ")\n";
    }
}

void inputHandler::setPlayerMap(std::map<int, std::shared_ptr<Player>>& updatedMap) {
    this->playerMap = updatedMap;
}


