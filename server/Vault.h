#pragma once

#include "User.h"

#include <string>
#include <vector>

struct DirEntry
{
    std::string filePath;
    int modDate;
};

class Vault
{
public:
    Vault(std::string name) : name {name} {}

    std::vector<DirEntry> getEntries() const;
    bool uploadFile();
    bool downloadFile();
    bool sync(Client& client, const std::string& syncOption);
    std::string getName() const {return name;}
    bool isClientConnected(const Client& client);

private:
    std::vector<User>::iterator getUser(const Client& client);

    std::vector<User> users;
    std::string name;
};