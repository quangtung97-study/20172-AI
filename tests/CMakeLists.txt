project(tests)

link_directories(../lib/test)

add_executable(test_main 
    test_example.cpp
    ai/game/ListTest.cpp
)

target_link_libraries(test_main
    src
    gmock_main
    pthread
)

add_subdirectory(ai/search)
add_subdirectory(ai/game)
add_subdirectory(ai/andor)
add_subdirectory(ai/solving)

ADD_TEST(NAME Main COMMAND test_main)
