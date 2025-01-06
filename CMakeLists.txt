cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death__sfml)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# SFML part
project(CMakeSFMLProject LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_subdirectory(src/logger)
add_subdirectory(src/topology)

add_executable(cpp_game_of_death__sfml src/main.cpp)
target_link_libraries(
    cpp_game_of_death__sfml PRIVATE SFML::Graphics
    logger
    topology
)
