#include "Game/Declarations/game.hpp"
#include <iostream>

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "     Starting Labyrinth Sprint Server" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "🧠 Waiting for frontend to send config (mode + difficulty)..." << std::endl;

    Game game;
    game.run();  // Starts WebSocket server and waits for config to trigger startGame()

    return 0;
}


