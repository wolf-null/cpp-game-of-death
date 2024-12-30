add_library(topology STATIC
        node.hpp
        conway.hpp
        test_node.hpp
        test_node.cpp
        test_conway.hpp
        test_conway.cpp
)

# A nasty way for including one static linked library to another.
target_include_directories(topology PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(topology logger)
