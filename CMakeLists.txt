cmake_minimum_required(VERSION 3.2)

project(AI)

set(CMAKE_CXX_STANDARD 14)

include_directories(./include)

add_subdirectory(src)

add_executable(main 
    src/main.cpp
)

target_link_libraries(main 
    src
)

enable_testing()
add_subdirectory(tests)
