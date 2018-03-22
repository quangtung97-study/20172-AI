project(src)

add_library(src 
    source.cpp
)

add_subdirectory(ai/game/gomoku)
add_subdirectory(ai/solving)
