add_library(topology STATIC
        node.hpp
        conway_node.hpp
        tests/test_node.hpp
        tests/test_conway.hpp
        grid.hpp tests/test_grid.hpp)

# A nasty way for including one static linked library to another.
target_include_directories(topology PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(topology logger)
