cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src/logger)
add_subdirectory(src/topology)

add_executable(cpp_game_of_death  src/main.cpp)

target_link_libraries(cpp_game_of_death PUBLIC src/logger)
target_link_libraries(cpp_game_of_death PUBLIC src/topology)

target_include_directories(
        cpp_game_of_death PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/src"
)
