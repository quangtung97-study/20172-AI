cmake_minimum_required(VERSION 3.2)

project(AI)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(./include)

add_subdirectory(src)

add_executable(play_tictactoe
    src/play_tictactoe.cpp
)

target_link_libraries(play_tictactoe 
    src
)

add_executable(play_gomoku
    src/ai/game/gomoku/main.cpp
)

target_link_libraries(play_gomoku
    ai-game-gomoku
)

add_executable(gomoku_gui
    src/ai/game/gomoku/main_SDLWrapper.cpp
)

target_link_libraries(gomoku_gui
    ai-game-gomoku-gui
)

add_executable(solving
    src/ai/solving/main.cpp
)

target_link_libraries(solving
    ai-solving
)

enable_testing()
add_subdirectory(tests)
