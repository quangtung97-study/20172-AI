project(ai-game-gomoku)

add_library(ai-game-gomoku 
    Heuristic.cpp
    InfiniteMatrix.cpp
    State.cpp
    MoveOrderer.cpp
)

add_library(ai-game-gomoku-gui
    AIMover.cpp
    MatrixRenderer.cpp
    SDLWrapper.cpp
)

target_link_libraries(ai-game-gomoku-gui
    ai-game-gomoku
    SDL2 
    SDL2_image
    SDL2_ttf
    pthread
)

