cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/logger)
add_subdirectory(src/topology)

add_executable(cpp_game_of_death src/main.cpp)

target_link_libraries(cpp_game_of_death
    logger
    topology
)
