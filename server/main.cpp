#include "Server.h"
#include "VaultManager.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
        Logger::LOG(ERROR, "Usage: ./server.out storage_path [port=1515]");

    std::string port = "1515";
    if (argc == 3)
        port = argv[2];
    
    // TODO: validate input

    Server server{port};
    VaultManager::getInstance()->setStoragePath(argv[1]);

    try
    {
        server.init();
    }
    catch (std::exception& e)
    {
        Logger::LOG(ERROR, e.what());
        return -1;
    }

    try
    {
        server.run();
    }
    catch (std::exception& e)
    {
        Logger::LOG(ERROR, e.what());
    }
    catch (...)
    {
        Logger::LOG(ERROR, "Unknown exception!");
    }

    return 0;
}