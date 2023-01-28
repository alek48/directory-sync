#include "Stage.h"
#include "Logger.h"

std::vector<std::string> getAvailableCommands(Stage stage)
{
    if (stage == Connected)
    {
        return std::vector<std::string> {
            "list [n=1]",
            "create [vault_name]",
            "sync [vault_name] safe",
            "sync [vault_name] force",
        };

    }
    else if (stage == Syncing)
    {
        return std::vector<std::string> {
            "request download [filepath]",
            "request upload [filepath]",
            "list entries"
        };
    }
    else
    {
        Logger::LOG(ERROR, "Wrong stage provided");
        return std::vector<std::string> {};
    }
}