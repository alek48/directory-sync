#include "Vault.h"
#include "VaultManager.h"
#include "User.h"
#include "Client.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <algorithm>

namespace fs = std::filesystem; // requires c++17

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
              + system_clock::now());
    return system_clock::to_time_t(sctp);
}

int getModDateEpoch(const fs::directory_entry& entry)
{
    auto timeEntry = fs::last_write_time(entry);
    time_t cftime = to_time_t(timeEntry);

    const auto sysClock = std::chrono::system_clock::from_time_t(cftime);
    return std::chrono::duration_cast<std::chrono::seconds>(
                   sysClock.time_since_epoch()).count();

}

Vault::Vault(std::string name) : 
    name {name},
    workingDirPath {VaultManager::getInstance()->storagePath + name + "/"}
{

}

std::vector<std::string> Vault::getFiles() const
{
    std::vector<std::string> files;
    std::string path = VaultManager::getInstance()->storagePath;

    for (const auto& entry : fs::recursive_directory_iterator(path+name))
    {   
        if (!fs::is_directory(entry.status())) // only files
        {
            files.push_back(entry.path());
        }
    }

    return files;
}

std::vector<DirEntry> Vault::getEntries() const
{
    std::vector<DirEntry> entries;
    std::string path = VaultManager::getInstance()->storagePath;
    for (const auto& entry : fs::recursive_directory_iterator(path+name))
    {   
        if (!fs::is_directory(entry.status())) // only files
        {
            DirEntry file;
            file.filePath = entry.path();
            file.modDate = getModDateEpoch(entry);

            entries.push_back(file);
        }
            
    }

    return entries;
}

std::vector<User>::iterator Vault::getUser(const Client& client)
{
    return std::find_if(users.begin(), users.end(), 
        [&client](const User& user){return user.client == &client;});
}

bool Vault::isClientConnected(const Client& client)
{
    return getUser(client) != users.end();
}

bool Vault::sync(Client& client, const std::string& syncOption)
{
    if (isClientConnected(client))
        return false;

    if (syncOption == "safe")
    {
        users.emplace_back(User{client, this});
        client.user = &(users.back());
        return true;
    }
    else if (syncOption == "force")
    {
        if (users.size() == 0)
        {
            users.emplace_back(User{client, this, true});
            client.user = &(users.back());
            return true;
        }
        return false;
    }
    else
        return false;
}

int Vault::createFile(const std::string& relativePath) const
{
    std::string path = workingDirPath + relativePath;
    return open(path.c_str(), O_WRONLY|O_CREAT|O_TRUNC);
}

int Vault::openFile(const std::string& relativePath) const
{
    std::string path = (workingDirPath + relativePath);
    return open(path.c_str(), O_RDONLY);
}