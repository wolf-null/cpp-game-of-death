add_library(topology STATIC
        node.hpp
        conway.hpp
        tests/test_node.hpp
        tests/test_node.cpp
        tests/test_conway.hpp
        tests/test_conway.cpp
)

# A nasty way for including one static linked library to another.
target_include_directories(topology PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(topology logger)
