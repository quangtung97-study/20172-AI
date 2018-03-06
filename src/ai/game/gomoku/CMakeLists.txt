project(ai-game-gomoku)

add_library(ai-game-gomoku 
    Heuristic.cpp
    InfiniteMatrix.cpp
    State.cpp
    MoveOrderer.cpp
)
