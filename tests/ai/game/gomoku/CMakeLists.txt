project(tests-ai-game-gomoku)

add_executable(test_ai_game_gomoku
    HeuristicTest.cpp
)

target_link_libraries(test_ai_game_gomoku
    ai-game-gomoku
    gmock_main
    pthread
)

ADD_TEST(NAME AIGameGomoku COMMAND test_ai_game_gomoku)
