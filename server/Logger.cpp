#include "Logger.h"

void Logger::LOG(LogType type, std::string message)
{
    switch (type)
        {
        case DEBUG:
            std::cout << "DEBUG: " << message << '\n';
            break;
        case INFO:
            std::cout << "INFO: " << message << '\n';
            break;
        case ERROR:
            std::cerr << "ERROR: " << message << '\n';
            break;
        
        default:
            std::cout << "UNKNOWN LogType!: " << message << '\n';
        }
}