project(tests)

link_directories(../lib/test)

add_executable(test_main 
    test_example.cpp
    ai/game/ListTest.cpp

    # Image Puzzle
    ai/search/imagepuzzle/StateTest.cpp
    ai/search/imagepuzzle/NodeTest.cpp

    ai/search/BreadthSearchTest.cpp
)

target_link_libraries(test_main
    src
    gmock_main
    pthread
)

ADD_TEST(NAME TestMain COMMAND test_main)
