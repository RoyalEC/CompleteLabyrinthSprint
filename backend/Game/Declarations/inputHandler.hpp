#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <iostream>
#include <tuple>
#include <map>
#include <memory>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>

class Player;
class Game;  // Forward declaration to allow config updates

typedef websocketpp::server<websocketpp::config::asio> server;
using json = nlohmann::json;

class inputHandler
{
private:
    std::unique_ptr<server> s;
    std::map<int, std::shared_ptr<Player>> playerMap;
    Game* game = nullptr; // Optional pointer for config updates

public:
    inputHandler();
    ~inputHandler();
    inputHandler(std::map<int, std::shared_ptr<Player>>& playerMap);
    inputHandler(inputHandler&&) = default;
    inputHandler& operator=(inputHandler&& other) noexcept;

    // Main WebSocket input processor
    std::pair<std::shared_ptr<Player>, std::string> handleWebSocketInput(const std::string& message);

    // Setup helpers
    void setGame(Game* gameInstance); // <-- for configuration messages
    void setPlayerMap(std::map<int, std::shared_ptr<Player>>& updatedMap); // ✅ NEW
    void addPlayer(int playerId, std::shared_ptr<Player> player);

    // Player movement routing
    void updateInput(std::shared_ptr<Player> player, const std::string& action);

private:
    std::shared_ptr<Player> getId(int playerId);
    std::tuple<int, int> parseInput(const std::string& inputType, const std::string& action);
    void handleInput(std::shared_ptr<Player> player, const std::string& action);
};

#endif // INPUTHANDLER_HPP

