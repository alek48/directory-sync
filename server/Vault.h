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
    int createFile(const std::string& relativePath) const;
    int openFile(const std::string& relativePath, int& fullFilesize) const;
    int deleteFile(const std::string& relativePath) const;
    bool uploadFilePart(int sockfd, std::vector<char> data);
    std::vector<char> downloadFilePart(int sockfd);

    std::vector<User*>::iterator getUser(const User& user);
    void removeUser(const User& user);

    std::vector<User*> users; // TODO: when users move Client loses pointer
    std::string name;
    std::string workingDirPath;
};