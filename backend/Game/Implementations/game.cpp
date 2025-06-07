#include "../Declarations/game.hpp"
#include "../Declarations/labyrinth.hpp"
#include "../Declarations/player.hpp"
#include "../Declarations/aiController.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

Game::Game()
    : isSinglePlayerMode(true), difficulty(EASY), currentLevel(0), handler(playerMap), aiThread(nullptr)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    handler.setGame(this);
}

void Game::setSinglePlayerMode(bool isSingle)
{
    isSinglePlayerMode = isSingle;
}

void Game::setDifficulty(const std::string& input)
{
    std::string level = input;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);

    if (level == "easy") difficulty = EASY;
    else if (level == "medium") difficulty = MEDIUM;
    else if (level == "hard") difficulty = HARD;
    else difficulty = EASY;

    std::cout << "Selected difficulty: " << level << std::endl;
}

void Game::run()
{
    websockerServer.set_reuse_addr(true);
    websockerServer.init_asio();

    websockerServer.set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::string message = msg->get_payload();
        handlePlayerMove(message);
        });

    websockerServer.set_open_handler([this](websocketpp::connection_hdl hdl) {
        connections.insert(hdl);
        });

    websockerServer.set_close_handler([this](websocketpp::connection_hdl hdl) {
        connections.erase(hdl);
        });

    websockerServer.listen(9002);
    websockerServer.start_accept();

    std::cout << "Server is running and ready to accept connections." << std::endl;
    websockerServer.run();
}

void Game::startGame()
{
    if (isSinglePlayerMode)
    {
        generateSinglePlayerLevels();
    }
    else
    {
        generateMultiplayerLevel();
    }

    setupPlayers();
    configReceived = true;
    gameOver = false;

    std::cout << "✅ Game started!" << std::endl;
    displayLabyrinth();
    broadcastGameState();

    // 🧠 Start AI thread if multiplayer
    if (!isSinglePlayerMode)
    {
        ai = std::make_unique<aiController>(playerMap[2], *labyrinth, difficulty);

        // Use simplified runAI with only isGameOver lambda
        aiThread = std::make_unique<std::thread>([this]() {
            ai->runAI([this]() {
                return this->isGameOver();
                });
            });
    }
}


void Game::broadcastGameState()
{
    if (!labyrinth)
    {
        std::cerr << "⚠️ Labyrinth not initialized. Cannot broadcast game state.\n";
        return;
    }

    nlohmann::json state;
    state["labyrinth"] = labyrinth->getLabyrinth();
    state["width"] = labyrinth->getWidth();
    state["height"] = labyrinth->getHeight();

    if (playerMap.count(1)) {
        auto player = playerMap[1];
        state["player"] = {
            {"x", player->getX()},
            {"y", player->getY()}
        };
    }

    if (playerMap.count(2)) {
        auto aiPlayer = playerMap[2];
        state["ai"] = {
            {"x", aiPlayer->getX()},
            {"y", aiPlayer->getY()}
        };
    }

    std::string payload = state.dump();

    for (const auto& hdl : connections)
    {
        websocketpp::lib::error_code ec;
        websockerServer.send(hdl, payload, websocketpp::frame::opcode::text, ec);
        if (ec)
        {
            std::cerr << "❌ Send failed: " << ec.message() << std::endl;
        }
    }

    // ✅ Check win conditions after sending game state
    if (gameOver) return;

    if (playerMap.count(1) && labyrinth->gameOver(*playerMap[1])) {
        broadcastWinMessage(1);
        return;
    }

    if (playerMap.count(2) && labyrinth->gameOver(*playerMap[2])) {
        broadcastWinMessage(2);
        return;
    }
}


void Game::setupPlayers()
{
    int startX = labyrinth->getStartX();
    int startY = labyrinth->getStartY();

    auto player = std::make_shared<Player>(1, 'P', startX, startY);
    addPlayer(1, player);

    handler.setPlayerMap(playerMap);

    // Add AI player if multiplayer
    if (!isSinglePlayerMode)
    {
        auto aiPlayer = std::make_shared<Player>(2, 'A', startX, startY);
        addPlayer(2, aiPlayer);
    }
}

void Game::generateSinglePlayerLevels()
{
    levels.clear();

    for (int i = 0; i < 5; i++) {
        int baseSize = 10;
        int variation = std::rand() % 5 + 1;
        int size = baseSize * difficulty + (i * 2) + variation;

        labyrinthMap map(size, size);
        map.generateLabyrinth();
        levels.emplace_back(std::move(map));
    }

    labyrinth = std::make_unique<labyrinthMap>(std::move(levels[0]));
    std::cout << "✅ Level generated with " << labyrinth->getLabyrinth().size() << " rows.\n";
}

void Game::generateMultiplayerLevel()
{
    int size = 20;
    labyrinth = std::make_unique<labyrinthMap>(size, size);
	labyrinth->generateLabyrinth();
}

labyrinthMap& Game::getCurrentlevel()
{
    return *labyrinth;
}

void Game::addPlayer(int playerId, std::shared_ptr<Player> player)
{
    handler.addPlayer(playerId, player);
    playerMap[playerId] = player;
    std::cout << "Added player with ID " << playerId << " to Game's playerMap." << std::endl;
}

void Game::handlePlayerMove(const std::string& message)
{
    std::cout << "Received message: " << message << std::endl;

    auto json = nlohmann::json::parse(message);

    if (json.contains("type") && json["type"] == "config") {
        if (gameOver || configReceived) {
            resetGame();
        }
        std::string mode = json.value("mode", "single");
        std::string diff = json.value("difficulty", "easy");
        if (mode == "single") {
            setSinglePlayerMode(true);   // 1 player only
        }
        else if (mode == "local") {
            setSinglePlayerMode(false);  // 1 player + AI
        }
        else {
            std::cerr << "⚠️ Unknown mode: " << mode << " — defaulting to single player.\n";
            setSinglePlayerMode(true);
        }

        setDifficulty(diff);
        startGame();
        return;
    }

    if (!configReceived) {
        std::cerr << "⚠️ Received non-config message before game was configured. Ignoring." << std::endl;
        return;
    }

    auto [player, action] = handler.handleWebSocketInput(message);
    handler.updateInput(player, action);
    broadcastGameState();
}

std::string Game::getGameState()
{
    nlohmann::json json;
    json["labyrinth"] = labyrinth->getLabyrinth();
    json["height"] = labyrinth->getHeight();
    json["width"] = labyrinth->getWidth();
    return json.dump();
}

bool Game::isSinglePlayer()
{
    return isSinglePlayerMode;
}

void Game::nextLevel()
{
    if (++currentLevel < levels.size())
    {
        labyrinth = std::make_unique<labyrinthMap>(std::move(levels[currentLevel]));
    }
    else
    {
        std::cout << "No more levels." << std::endl;
    }
}

void Game::displayLabyrinth()
{
    std::cout << "Level " << (currentLevel + 1) << std::endl;
    labyrinth->printLabyrinth();
    std::cout << "Printing labyrinth" << std::endl;
}

void Game::moveToNewLevel(int levelIndex)
{
    if (levelIndex >= 0 && levelIndex < levels.size())
    {
        labyrinth = std::make_unique<labyrinthMap>(std::move(levels[levelIndex]));
        currentLevel = levelIndex;
    }
}

void Game::broadcastWinMessage(int playerId)
{
    gameOver = true;

    nlohmann::json message;
    message["type"] = "gameOver";
    message["winner"] = playerId;

    std::string payload = message.dump();

    for (const auto& hdl : connections)
    {
        websocketpp::lib::error_code ec;
        websockerServer.send(hdl, payload, websocketpp::frame::opcode::text, ec);
        if (ec)
        {
            std::cerr << "❌ Failed to send game over message: " << ec.message() << std::endl;
        }
    }

    // Clean up AI thread
    if (aiThread && aiThread->joinable()) {
        ai->stop();
        aiThread->join();
    }
}

void Game::resetGame()
{
    std::cout << "🔄 Resetting game state..." << std::endl;

    if (aiControllerPtr) {
        aiControllerPtr->stop();  // Stop AI loop
    }
    if (aiThread && aiThread->joinable()) {
        aiThread->join();         // Join thread safely
    }

    aiControllerPtr.reset();
    aiThread.reset();
    aiAIPlayer.reset();

    labyrinth.reset();
    levels.clear();
    playerMap.clear();
    configReceived = false;
    gameOver = false;
    currentLevel = 0;
}

