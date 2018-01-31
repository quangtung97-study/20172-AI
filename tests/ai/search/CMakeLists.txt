project(tests-ai-search)

add_executable(test_ai_search
    imagepuzzle/StateTest.cpp
    imagepuzzle/NodeTest.cpp
    BreadthSearchTest.cpp
)

target_link_libraries(test_ai_search
    src
    gmock_main
    pthread
)

ADD_TEST(NAME TestAISearch COMMAND test_ai_search)
