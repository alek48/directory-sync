#include "Logger.h"

void Logger::LOG(LogType type, std::string message)
{
    switch (type)
        {
        case DEBUG:
            std::cout << "DEBUG:\t" << message << '\n';
            break;
        case INFO:
            std::cout << "INFO:\t" << message << '\n';
            break;
        case ERROR:
            std::cerr << "ERROR:\t" << message << '\n';
            break;
        
        default:
            std::cout << "UNKNOWN LogType!:\t" << message << '\n';
        }
}