project(tests-ai-search)

add_executable(test_ai_search
    imagepuzzle/StateTest.cpp
    imagepuzzle/NodeTest.cpp
)

add_executable(test_ai_depth_search
    DepthSearchTest.cpp
)

add_executable(test_ai_breadth_search
    BreadthSearchTest.cpp
)

target_link_libraries(test_ai_search
    src
    gmock_main
    pthread
)

target_link_libraries(test_ai_depth_search
    src
    gmock_main
    pthread
)

target_link_libraries(test_ai_breadth_search
    src
    gmock_main
    pthread
)

ADD_TEST(NAME Search COMMAND test_ai_search)
ADD_TEST(NAME DepthSearch COMMAND test_ai_depth_search)
ADD_TEST(NAME BreadthSearch COMMAND test_ai_breadth_search)
