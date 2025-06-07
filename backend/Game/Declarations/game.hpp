#ifndef GAME_HPP
#define GAME_HPP

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <thread>

#include "labyrinth.hpp"
#include "player.hpp"
#include "inputHandler.hpp"
#include "Difficulty.hpp"
#include "aiController.hpp"  // <-- Added for AI support
#include <nlohmann/json.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

class Game
{
private:
    bool isSinglePlayerMode = true;
    bool configReceived = false;
    bool gameOver = false;
    Difficulty difficulty = EASY;

    std::unique_ptr<labyrinthMap> labyrinth;
    std::vector<labyrinthMap> levels;
    server websockerServer;
    int currentLevel = 0;

    std::map<int, std::shared_ptr<Player>> playerMap;
    inputHandler handler;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;

    std::unique_ptr<aiController> ai;            // <-- AI controller
    std::shared_ptr<Player> aiAIPlayer;
    std::unique_ptr<aiController> aiControllerPtr;
    std::unique_ptr<std::thread> aiThread;
    // <-- Background thread for AI

    void generateSinglePlayerLevels();
    void generateMultiplayerLevel();
    std::string getGameState();

public:
    Game();

    void setSinglePlayerMode(bool isSingle);
    void setDifficulty(const std::string& input);
    void startGame();

    void run();
    void nextLevel();
    labyrinthMap& getCurrentlevel();
    void handlePlayerMove(const std::string& message);
    void broadcastGameState();

    std::string getPlayerInput(int playerId);
    void addPlayer(int playerId, std::shared_ptr<Player> player);
    bool isSinglePlayer();
    void setupPlayers();
    void displayLabyrinth();
    void broadcastWinMessage(int playerId);
    void moveToNewLevel(int levelIndex);

    void resetGame();
    bool isGameOver() const { return gameOver; }
    void setGameOver(bool over) { gameOver = over; }
};

#endif // GAME_HPP



