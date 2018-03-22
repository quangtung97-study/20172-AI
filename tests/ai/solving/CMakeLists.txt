project(tests-ai-solving)

add_executable(test_ai_solving
    ZOLTest.cpp
)

target_link_libraries(test_ai_solving
    ai-solving
    gmock_main
    pthread
)

ADD_TEST(NAME AISolving COMMAND test_ai_solving)
