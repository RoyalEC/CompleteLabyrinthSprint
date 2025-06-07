cmake_minimum_required(VERSION 3.15)
project(LabyrinthSprint)

set(CMAKE_CXX_STANDARD 17)

# Include local header-only dependencies
include_directories(
    ${CMAKE_SOURCE_DIR}/third_party/websocketpp/websocketpp
    ${CMAKE_SOURCE_DIR}/third_party/json
)

# For Boost (only system, not asio!)
find_package(Boost REQUIRED COMPONENTS system)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# For standalone ASIO
find_package(asio REQUIRED)

# Define required macros
add_compile_definitions(
    ASIO_STANDALONE
    BOOST_ASIO_NO_DEPRECATED
    _WEBSOCKETPP_CPP11_STL_
    _WEBSOCKETPP_NO_EXCEPTIONS_
    BOOST_ERROR_CODE_HEADER_ONLY
)

add_executable(LabyrinthSprint
    main.cpp
    Game/Implementations/inputHandler.cpp
    Game/Implementations/player.cpp
    Game/Implementations/labyrinth.cpp
    Game/Implementations/game.cpp
    Game/Implementations/aiController.cpp
)

# Link with correct targets
target_link_libraries(LabyrinthSprint
    PRIVATE
        Boost::system
        asio
)













