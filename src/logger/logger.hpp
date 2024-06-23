#ifndef CPP_GAME_OF_DEATH_LOGGER_HPP
#define CPP_GAME_OF_DEATH_LOGGER_HPP

#include <string>
#include <vector>

constexpr int LOG_LEVEL_NOT_SET = -1;
constexpr int LOG_LEVEL_DEBUG = 10;
constexpr int LOG_LEVEL_INFO = 20;
constexpr int LOG_LEVEL_WARNING = 30;
constexpr int LOG_LEVEL_ERROR = 40;
constexpr int LOG_LEVEL_CRITICAL = 50;

struct LogRecord {
    std::string message;
    int level{LOG_LEVEL_NOT_SET};

    LogRecord() = default;
    LogRecord(std::string && message, int level) : message{message}, level{level} {} ;
    ~LogRecord() = default;

    LogRecord(LogRecord& base) = default;
    LogRecord(LogRecord&& base) = default;
    LogRecord& operator = (LogRecord&& base) = default;
};


class LogHandler {
    int level_{LOG_LEVEL_NOT_SET};

public:
    LogHandler() = default;
    LogHandler(int level) : level_{level} {};
    virtual ~LogHandler() = default;

    void set_level(int level = LOG_LEVEL_NOT_SET);
    int get_level() const;

    virtual void emit(LogRecord& record) = 0;
};

struct LoggerLogLevelHelper;

struct Logger {
private:
    int log_level_ {LOG_LEVEL_NOT_SET};

public:
    Logger(int log_level = LOG_LEVEL_NOT_SET) : log_level_{log_level} {};

    std::vector <LogHandler*> handlers;

    void set_level(int level = LOG_LEVEL_NOT_SET);
    int get_level() const;

    void add_handler(LogHandler * handler);
    void remove_handler(LogHandler * handler);

    void emit(LogRecord record);
    LoggerLogLevelHelper log(int level = LOG_LEVEL_NOT_SET);
};


struct LoggerLogLevelHelper {
    Logger * origin;
    int log_level;
    LoggerLogLevelHelper(Logger * origin, int log_level) : origin{origin}, log_level{log_level} {};

    LoggerLogLevelHelper& operator << (std::string && msg);
    LoggerLogLevelHelper& operator << (const char * msg);
    LoggerLogLevelHelper& operator << (LogRecord && record);
};

#endif //CPP_GAME_OF_DEATH_LOGGER_HPP
