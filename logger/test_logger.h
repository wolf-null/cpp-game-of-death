#ifndef CPP_GAME_OF_DEATH_TEST_LOGGER_H
#define CPP_GAME_OF_DEATH_TEST_LOGGER_H

#include <array>
#include <cassert>
#include <iostream>

#include "logger.h"

void test_LogRecord() {
    LogRecord rec_01{"test message", LOG_LEVEL_INFO};
    LogRecord rec_02{rec_01};
    assert(rec_02.message == "test message");
    assert(rec_02.level == LOG_LEVEL_INFO);

    LogRecord rec_03{LogRecord{"also test message", LOG_LEVEL_WARNING}};
    assert(rec_03.message == "also test message");
    assert(rec_03.level == LOG_LEVEL_WARNING);
}

class TestLogHandler_ : public LogHandler {
    LogRecord last_record_{"", LOG_LEVEL_NOT_SET};
public:
    TestLogHandler_() : LogHandler() {}
    TestLogHandler_(int level) : LogHandler(level) {};
    void emit(LogRecord& record) override {
        last_record_ = LogRecord{record};
    }
    LogRecord last_record() {
        return last_record_;
    }
};

void test_LogHandler() {
    auto * origin_handler = new TestLogHandler_{LOG_LEVEL_WARNING};
    LogHandler * handler = dynamic_cast<LogHandler*>(origin_handler);
    assert(handler->get_level() == LOG_LEVEL_WARNING);

    LogRecord test_record{"test msg", LOG_LEVEL_ERROR};
    handler->emit(test_record);
    auto last_record = origin_handler->last_record();
    assert(last_record.message == "test msg");
    assert(last_record.level == LOG_LEVEL_ERROR);

    origin_handler->set_level(LOG_LEVEL_DEBUG);
    assert(origin_handler->get_level() == LOG_LEVEL_DEBUG);
}

void test_Logger_levels(int logger_level, int handler_level, int message_level, bool print = true) {
    assert(logger_level > 0 and handler_level > 0 and message_level > 0); // Test args bounds expectations.
    assert(logger_level <= 100 and handler_level <= 100 and message_level <= 100); // Test args bounds expectations.

    bool shall_pass =
        logger_level <= message_level and
        handler_level <= message_level;

    if (print)
        std::cout << "loglevel:" << logger_level << "\thndlevel:" << handler_level << "\tmsglevel:" << message_level <<
        "\tshallpass:" << int(shall_pass);

    Logger logger{logger_level};
    TestLogHandler_ handler{handler_level};

    auto none_msg_str = "";
    auto none_msg_level = LOG_LEVEL_NOT_SET;

    auto test_msg_str = "test message";
    LogRecord test_msg{test_msg_str, message_level};

    logger.add_handler(&handler);
    logger.emit(test_msg);

    auto last_msg = handler.last_record();

    if (shall_pass) {
        if (print) std::cout << "\tPASS";
        assert(last_msg.message == test_msg_str);
        assert(last_msg.level == message_level);
    } else {
        if (print) std::cout << "\tNO-PASS";
        assert(last_msg.message == none_msg_str);
        assert(last_msg.level == none_msg_level);
    }
    if (print) std::cout << "\tOK\n";
}

void test_Logger(bool print = true) {
    std::array<int, 3> levels {LOG_LEVEL_DEBUG, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR};

    for (int logger_level : levels)
        for (int handler_level : levels)
            for (int message_level : levels)
                test_Logger_levels(
                    logger_level,
                    handler_level,
                    message_level,
                    print
                );
}

void test_LoggerLogLevelHelper(
    int message_level,
    int hlp_level,
    int mode,
    bool print = true
) {
    if (print)
        std::cout << "message_level:" << message_level << "\thlp_level:" << hlp_level << "\tmode:" << mode;

    auto base_level = LOG_LEVEL_DEBUG;
    Logger logger{base_level};
    TestLogHandler_ handler{base_level};
    logger.add_handler(&handler);

    auto test_msg_str = "test message";
    auto test_msg_level = message_level;
    auto helper = logger.log(hlp_level);

    switch (mode) {
        case 1:
            helper << test_msg_str;
            test_msg_level = helper.log_level;
            break;
        case 2:
            helper << std::string(test_msg_str);
            test_msg_level = helper.log_level;
            break;
        case 3:
            helper << LogRecord(test_msg_str, message_level);
            break;
        default:
            throw;
    }

    auto received_msg = handler.last_record();

    if (
        helper.log_level != -1 and
        helper.log_level >= logger.get_level() or
        helper.log_level == -1 and
        test_msg_level >= logger.get_level() and
        test_msg_level >= handler.get_level()
    )
        assert(received_msg.message == test_msg_str);
    else
        assert(received_msg.message == "");

    assert(helper.log_level != LOG_LEVEL_NOT_SET ? received_msg.level == helper.log_level: test_msg_level);

    if (print)
        std::cout << "\tOK\n";
}

void test_LoggerLogLevelHelper(bool print = true) {
    std::array <int, 3> levels {LOG_LEVEL_NOT_SET, LOG_LEVEL_DEBUG, LOG_LEVEL_WARNING};
    std::array <int, 3> modes {1, 2, 3};

    for (auto message_level : levels)
        for (auto hlp_level : levels)
            for (auto mode: modes)
                test_LoggerLogLevelHelper(message_level, hlp_level, mode, print);
}

#endif //CPP_GAME_OF_DEATH_TEST_LOGGER_H

