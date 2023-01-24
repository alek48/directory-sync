#include "Server.h"
#include "Logger.h"

int main()
{
    Server server{};
    
    try
    {
        server.init();
    }
    catch (std::exception e)
    {
        Logger::LOG(ERROR, e.what());
        return -1;
    }

    try
    {
        server.run();
    }
    catch (std::exception e)
    {
        Logger::LOG(ERROR, e.what());
    }
    catch (...)
    {
        Logger::LOG(ERROR, "Unknown exception!");
    }

    return 0;
}