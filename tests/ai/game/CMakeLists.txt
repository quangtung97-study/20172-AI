project(tests-ai-game)

add_executable(test_ai_game_tictactoe
    tictactoe/StateTest.cpp
)

target_link_libraries(test_ai_game_tictactoe
    src
    gmock_main
    pthread
)

ADD_TEST(NAME TicTacToe COMMAND test_ai_game_tictactoe)
