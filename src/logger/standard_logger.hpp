#ifndef CPP_GAME_OF_DEATH_STANDARD_LOGGER_HPP
#define CPP_GAME_OF_DEATH_STANDARD_LOGGER_HPP

#include "logger.hpp"


class StdoutHandler : public LogHandler {
public:
    StdoutHandler() = default;
    StdoutHandler(int level) : LogHandler(level) {};
    ~StdoutHandler() override = default;

    void emit(LogRecord& record) override;
};


Logger* build_standard_logger(int log_level = LOG_LEVEL_DEBUG);

constexpr int DEFAULT_LOG_LEVEL = LOG_LEVEL_DEBUG;
static Logger * default_logger = build_standard_logger(DEFAULT_LOG_LEVEL);

static auto debug = default_logger->log(LOG_LEVEL_DEBUG);

#endif //CPP_GAME_OF_DEATH_STANDARD_LOGGER_HPP
