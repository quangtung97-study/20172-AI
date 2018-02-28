project(tests-ai-andor)

add_executable(test_ai_andor_astar_search
    AStarSearchTest.cpp
)

target_link_libraries(test_ai_andor_astar_search
    src
    gmock_main
    pthread
)

ADD_TEST(NAME AndOrAStarSearch COMMAND test_ai_andor_astar_search)
