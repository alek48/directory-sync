#include "User.h"
#include "Client.h"
#include "Logger.h"
#include "Vault.h"
#include "VaultManager.h"

#include <string>
#include <algorithm>

User::User(Client& client, Vault* vault, bool privileged) :
        client {&client}, vault {vault}, privileged {privileged}
{
}

User::~User()
{
    client->user = nullptr;
}

bool User::createFile(const std::string& path) // path relative to vault
{
    if (fileFd != -1) // only one file at a time
        return false;

    if (!privileged)
    {
        std::vector<std::string> files = vault->getFiles();
        auto found = std::find(files.begin(), files.end(), path);

        if (found != files.end())
            return false;
    }

    int fd = vault->createFile(path);
    if (fd == -1)
    {
        Logger::LOG(ERROR, "Could not create file: " + path);
        return false;
    }
    fileFd = fd;

    return true;
}

// open file for download
bool User::openFile(const std::string& relativePath) // path relative to vault
{
    if (fileFd != -1) // only one file at a time
        return false;

    std::vector<std::string> files = vault->getFiles();
    auto found = std::find(files.begin(), files.end(), vault->workingDirPath + relativePath);
    if (found == files.end()) // there is no such file
        return false;

    int fd = vault->openFile(relativePath);
    if (fd == -1)
    {
        Logger::LOG(ERROR, "Could not open file: " + relativePath);
        return false;
    }
    fileFd = fd;

    return true;
}
