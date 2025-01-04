add_library(topology STATIC
        node.hpp
        conway_node.hpp
        tests/test_node.hpp
        tests/test_node.cpp
        tests/test_conway.hpp
        tests/test_conway.cpp
        conway_topology.cpp conway_topology.hpp conway_node.cpp grid.cpp grid.hpp tests/test_grid.cpp tests/test_grid.hpp)

# A nasty way for including one static linked library to another.
target_include_directories(topology PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(topology logger)
