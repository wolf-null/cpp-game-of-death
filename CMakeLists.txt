cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        cpp_game_of_death
        main.cpp
        topology/node.hpp
        topology/test_node.cpp
        topology/test_node.hpp
        topology/conway.hpp
        topology/test_conway.hpp
        topology/test_conway.cpp
        logger/logger.cpp logger/logger.h
        logger/test_logger.h logger/standard_logger.cpp logger/standard_logger.hpp
)
