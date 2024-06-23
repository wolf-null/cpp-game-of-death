cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death)

add_library(
        topology
        node.hpp
        test_node.hpp
        test_node.cpp
        conway.hpp
        test_conway.hpp
        test_conway.cpp
)
