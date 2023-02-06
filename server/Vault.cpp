#include "Vault.h"
#include "VaultManager.h"
#include "User.h"
#include "Client.h"
#include "Logger.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

bool createDirectoryRecuresive(const std::string& dirName)
{
    if (!fs::create_directories(dirName))
    {
        if (fs::exists(dirName))
        {
            return true;    // directory already exists
        }

        Logger::LOG(ERROR, "createDirectoryRecuresive Failed to create " + dirName);
        return false;
    }
    return true;
}

int getFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

Vault::Vault(std::string name) : 
    name {name},
    workingDirPath {VaultManager::getInstance()->storagePath + name + "/"}
{

}

std::vector<std::string> Vault::getFiles() const
{
    std::vector<std::string> files;
    std::string vaultPath = VaultManager::getInstance()->storagePath + name;

    for (const auto& entry : fs::recursive_directory_iterator(vaultPath))
    {   
        if (!fs::is_directory(entry.status())) // only files
        {
            std::string serverRelativePath = entry.path();
            files.push_back(serverRelativePath.substr(vaultPath.size()+1, -1));
        }
    }

    return files;
}

std::vector<DirEntry> Vault::getEntries() const
{
    std::vector<DirEntry> entries;
    std::string vaultPath = VaultManager::getInstance()->storagePath + name;
    for (const auto& entry : fs::recursive_directory_iterator(vaultPath))
    {   
        if (!fs::is_directory(entry.status())) // only files
        {
            DirEntry file;
            std::string serverRelativePath = entry.path();
            file.filePath = serverRelativePath.substr(vaultPath.size()+1, -1);
            file.modDate = getModDateEpoch(entry);

            entries.push_back(file);
        }
            
    }

    return entries;
}

std::vector<User*>::iterator Vault::getUser(const User& user)
{
    return std::find_if(users.begin(), users.end(), 
        [&user](const User* u){return u == &user;});
}

bool Vault::isClientConnected(const Client& client)
{
    if (client.isConnected())
        return client.user->vault == this;
    else
        return false;
}

void Vault::removeUser(const User& user)
{
    users.erase(getUser(user));
}

bool Vault::sync(Client& client, const std::string& syncOption)
{
    if (isClientConnected(client))
        return false;

    if (syncOption == "safe")
    {
        client.user = std::make_unique<User>(client, this);
        users.push_back(client.user.get());
        return true;
    }
    else if (syncOption == "force")
    {
        if (users.size() == 0)
        {
            client.user = std::make_unique<User>(client, this, true);
            users.push_back(client.user.get());
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
    createDirectoryRecuresive(path.substr(0, path.find_last_of("/")));
    return open(path.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
}

int Vault::openFile(const std::string& relativePath, int& fullFilesize) const
{
    std::string path = (workingDirPath + relativePath);
    fullFilesize = getFileSize(path);
    return open(path.c_str(), O_RDONLY);
}

int Vault::deleteFile(const std::string& relativePath) const
{
    std::string path = (workingDirPath + relativePath);
    return unlink(path.c_str());

    // TODO: remove empty directories recursively
}

bool Vault::uploadFilePart(int sockfd, std::vector<char> data)
{
    int total {0};
    while (total < static_cast<int>(data.size()))
    {
        int n = write(sockfd, data.data(), data.size());
        
        if (n == -1)
        {
            Logger::LOG(ERROR, "Failed while writing file");
            return false;
        }
        else if (n == 0)
        {
            Logger::LOG(ERROR, "Write to file returned 0");
            return false;
        }
        else
            total += n;
    }

    return true;
}

std::vector<char> Vault::downloadFilePart(int sockfd)
{
    std::vector<char> data;
    char buf[4096];
    int n = read(sockfd, &buf, 4096);
    for (int i = 0; i < n; i++)
        data.push_back(buf[i]);

    return data;
}