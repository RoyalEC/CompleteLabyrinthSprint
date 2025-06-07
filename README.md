# 🧩 Labyrinth Sprint

**Labyrinth Sprint** is a grid-based maze game where a human player navigates a randomly generated labyrinth to reach the goal. The game supports:
- 🧍 Single-player mode
- 🧠 Local multiplayer mode vs AI
- 🌐 Planned future support for online multiplayer

Built with:
- C++ backend using WebSocket++, Boost.Asio, and nlohmann::json
- React Native frontend with WebSocket communication
- AI opponent with adjustable difficulty levels: Easy, Medium, Hard

---

## 📂 Project Structure

LabyrinthSprint/
├── backend/
│ ├── Game/
│ │ ├── Declarations/ # All header files (Player, Map, AI, etc.)
│ │ ├── Implementations/ # All .cpp logic implementations
│ ├── main.cpp # WebSocket server entry point
│ ├── CMakeLists.txt # CMake config
│
├── frontend/
│ ├── components/ # Reusable UI components
│ ├── contexts/ # WebSocketContext.js for shared state
│ ├── screens/ # HomeScreen, DifficultyScreen, GameScreen, etc.
│ ├── App.js # React Native navigation entry
│
└── README.md

Please Note:
The app is hosted on Render using their free tier service. As a result, the backend server may enter a "sleep mode" when not in use.
It may take up to 2 minutes for the server to spin up when first accessed. After this brief delay, the app will function normally.