#pragma once

#include <string>

class Client;
class Vault;

class User // client in vault
{
public:
    User(Client& client, Vault* vault, bool privileged = false);
    ~User();

    Client* const client;
    const Vault* const vault;
    bool privileged;
    int fileFd = -1;

    bool createFile(const std::string& path);
    bool openFile(const std::string& path);
    void uploadFile();
    void downloadFile();
};