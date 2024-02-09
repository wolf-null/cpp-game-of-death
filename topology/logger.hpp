#ifndef GAMEOFDEATH_LOGGER_H
#define GAMEOFDEATH_LOGGER_H

#include <iostream>
#include <array>


struct TinyLogger {
    TinyLogger() = default;
    ~TinyLogger() = default;
    std::ostream& operator <<(auto msg){
        return std::cout << msg;
    }
};

static TinyLogger logger;

#endif //GAMEOFDEATH_LOGGER_H
