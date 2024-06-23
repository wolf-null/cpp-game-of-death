cmake_minimum_required(VERSION 3.22)
project(cpp_game_of_death)

add_library(
        logger
        logger.hpp
        logger.cpp
        standard_logger.hpp
        standard_logger.cpp
        test_logger.h
)
