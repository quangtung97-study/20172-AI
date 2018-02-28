project(tests-ai-game)

add_executable(test_ai_game_tictactoe
    tictactoe/StateTest.cpp
)

add_executable(test_ai_game_minimax
    MinimaxTest.cpp
)

target_link_libraries(test_ai_game_tictactoe
    src
    gmock_main
    pthread
)

target_link_libraries(test_ai_game_minimax
    src
    gmock_main
    pthread
)

add_subdirectory(gomoku)

ADD_TEST(NAME TicTacToe COMMAND test_ai_game_tictactoe)
ADD_TEST(NAME Minimax COMMAND test_ai_game_minimax)
