#include <iostream>

#include "topology/node.hpp"
#include "topology/tests/test_node.hpp"
#include "topology/conway.hpp"
#include "topology/tests/test_conway.hpp"
#include "logger/tests/test_logger.h"

int main() {
    test_node();
    test_conway();
    test_LogRecord();
    test_LogHandler();
    test_Logger(false);
    test_LoggerLogLevelHelper(false);
    return 0;
}
