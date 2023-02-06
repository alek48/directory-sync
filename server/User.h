#pragma once

#include <string>
#include <vector>

class Client;
class Vault;
class FilePartMessage;
struct DirEntry;

enum Operation
{
    Inactive, Listing, Downloading, Uploading
};

class User // client in vault
{
public:
    User(Client& client, Vault* vault, bool privileged = false);
    
    ~User();

    Client* const client;
    Vault* const vault;
    bool privileged;
    Operation currentOperation = Inactive;

    bool startUpload(const std::string& path);
    bool startDownload(const std::string& path);
    bool startListEntries();
    void saveNextFilePart(FilePartMessage filePartMessage);
    void sendNextFilePart();
    void sendNextDirPart();

private:
    std::vector<DirEntry> allDirEntries;
    int entriesSent = 0;

    int fileFd = -1;
    int fullFileSize = 0;
    int currentFileSize = 0;
    std::string currentFilePath = "";
};