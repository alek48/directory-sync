#pragma once

#include <iostream>

enum LogType
{
    DEBUG, INFO, ERROR
};

class Logger
{
public:
    Logger() = delete;
    static void LOG(LogType type, std::string message);
};