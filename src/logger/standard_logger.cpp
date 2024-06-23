#include <iostream>

#include "standard_logger.hpp"

void StdoutHandler::emit(LogRecord &record) {
    std::cout << record.message;
}

Logger* build_standard_logger(int log_level) {
    auto * logger = new Logger{log_level};
    auto * std_handler = new StdoutHandler(log_level);
    logger->add_handler(std_handler);
    return logger;
}
