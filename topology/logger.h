#ifndef GAMEOFDEATH_LOGGER_H
#define GAMEOFDEATH_LOGGER_H

#include <iostream>
#include <array>


struct Logger {
    Logger() = default;
    ~Logger() = default;
    std::ostream& operator <<(auto msg){
        return std::cout << msg;
    }
};

static Logger logger;

#endif //GAMEOFDEATH_LOGGER_H
