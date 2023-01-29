#pragma once

#include <vector>
#include <string>

class User;
class Client;

struct DirEntry
{
    std::string filePath;
    int modDate;
};

class Vault
{
public:
    Vault(std::string name);

    std::vector<std::string> getFiles() const;
    std::vector<DirEntry> getEntries() const;
    bool sync(Client& client, const std::string& syncOption);
    std::string getName() const {return name;}
    bool isClientConnected(const Client& client);

    friend class User;

private:
    int createFile(const std::string& path) const;
    int openFile(const std::string& path) const;
    bool uploadFilePart(const std::string& path, std::vector<char> data);

    std::vector<User>::iterator getUser(const Client& client);

    std::vector<User> users;
    std::string name;
    std::string workingDirPath;
};