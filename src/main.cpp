#include "topology/tests/test_node.hpp"
#include "topology/tests/test_grid.hpp"
#include "topology/tests/test_conway.hpp"
#include "logger/tests/test_logger.h"

int main() {
    test_node();
    test_conway();
    test_LogRecord();
    test_LogHandler();
    test_Logger(false);
    test_LoggerLogLevelHelper(false);

    {
        using namespace topology::grid;
        test_grid(3, 3).perform_tests();
        test_grid(5, 3).perform_tests();
        test_grid(3, 5, GridTopology::TORUS).perform_tests();
    }
    return 0;
}
