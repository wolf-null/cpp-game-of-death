#include "logger.hpp"

// ---------------------------------------------------- LogHandler -----------------------------------------------------

void LogHandler::set_level(int level) {
    level_ = level;
}

int LogHandler::get_level() const {
    return level_;
}

void LogHandler::emit(LogRecord &record) {
}

// ------------------------------------------------------ Logger -------------------------------------------------------

void Logger::set_level(int level) {
    log_level_ = level;
}

int Logger::get_level() const {
    return log_level_;
}

void Logger::add_handler(LogHandler * handler) {
    /*TODO: Sort on add*/
    handlers.push_back(handler);
}

void Logger::remove_handler(LogHandler * handler) {
    std::remove(handlers.begin(), handlers.end(), handler);
}

void Logger::emit(LogRecord record) {
    if (record.level < get_level())
        return;

    for(auto * handler : handlers) {
        /*TODO: Break when handlers are sorted.*/
        if (record.level < handler->get_level())
            continue;
        handler->emit(record);
    }
}

LoggerLogLevelHelper Logger::log(int level) {
    return LoggerLogLevelHelper {
        this,
        level
    };
}

// ------------------------------------------------ LoggerLogLevelHelper ----------------------------------------------------

LoggerLogLevelHelper& LoggerLogLevelHelper::operator << (std::string && msg) {
    this->operator << (
            LogRecord {
                    std::forward<std::string> (msg),
                    log_level
            }
    );
    return *this;
}

LoggerLogLevelHelper& LoggerLogLevelHelper::operator << (const char * msg) {
    this->operator << (
        LogRecord {
            std::string (msg),
            log_level
        }
    );
    return *this;
}

LoggerLogLevelHelper& LoggerLogLevelHelper::operator << (LogRecord && record) {
    if (log_level != LOG_LEVEL_NOT_SET and record.level != log_level) {
        auto proxy_record = LogRecord{record};
        proxy_record.level = log_level;
        origin->emit(proxy_record);
    }
    else
        origin->emit(record);
    return *this;
}
