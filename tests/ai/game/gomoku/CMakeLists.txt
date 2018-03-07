project(tests-ai-game-gomoku)

add_executable(test_ai_game_gomoku
    HeuristicTest.cpp
    InfiniteMatrixTest.cpp
    StateTest.cpp
    MoveOrdererTest.cpp
)

target_link_libraries(test_ai_game_gomoku
    ai-game-gomoku
    gmock_main
    pthread
)

add_executable(test_ai_game_gomoku_gui
    GUITest.cpp
    AIMoverTest.cpp
)

target_link_libraries(test_ai_game_gomoku_gui
    ai-game-gomoku-gui
    gmock_main
    pthread
)

ADD_TEST(NAME AIGameGomoku COMMAND test_ai_game_gomoku)
ADD_TEST(NAME AIGameGomokuGUI COMMAND test_ai_game_gomoku_gui)
