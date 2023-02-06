#include "User.h"
#include "Client.h"
#include "Logger.h"
#include "Vault.h"
#include "VaultManager.h"
#include "Message.h"
#include "MessageManager.h"

#include <unistd.h>

#include <string>
#include <vector>
#include <algorithm>

User::User(Client& client, Vault* vault, bool privileged) :
        client {&client}, vault {vault}, privileged {privileged}
{
}

User::~User()
{
    vault->removeUser(*this);

    if (fileFd != -1)
        close(fileFd);

    if (currentOperation == Uploading)
    {
        vault->deleteFile(currentFilePath);
    }
}

// create file for upload
bool User::startUpload(const std::string& path) // path relative to vault
{
    if (fileFd != -1) // only one file at a time
        return false;

    if (currentOperation != Inactive)
    {
        Logger::LOG(ERROR, "User is busy");
        return false;
    }

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
        perror("startUpload");
        return false;
    }
    fileFd = fd;
    currentFileSize = 0;
    currentFilePath = path;

    currentOperation = Uploading;
    return true;
}

// open file for download
bool User::startDownload(const std::string& relativePath) // path relative to vault
{
    if (fileFd != -1) // only one file at a time
        return false;

    if (currentOperation != Inactive)
    {
        Logger::LOG(ERROR, "User is busy");
        return false;
    }

    std::vector<std::string> files = vault->getFiles();
    auto found = std::find(files.begin(), files.end(), vault->workingDirPath + relativePath);
    if (found == files.end()) // there is no such file
        return false;

    int fd = vault->openFile(relativePath, fullFileSize); // sets fullFileSize
    if (fd == -1)
    {
        Logger::LOG(ERROR, "Could not open file: " + relativePath);
        return false;
    }
    fileFd = fd;
    currentFileSize = 0;

    currentOperation = Downloading;
    return true;
}

bool User::startListEntries()
{
    if (currentOperation != Inactive)
    {
        Logger::LOG(ERROR, "User is busy");
        return false;
    }
    allDirEntries = vault->getEntries();
    entriesSent = 0;

    currentOperation = Listing;
    return true;
}

void User::saveNextFilePart(FilePartMessage filePartMessage)
{
    if (currentOperation != Uploading)
    {
        Logger::LOG(ERROR, "User can't upload");
        return;
    }

    vault->uploadFilePart(fileFd, filePartMessage.partData);

    currentFileSize += filePartMessage.partSize;
    if (filePartMessage.fullSize == currentFileSize)
    {
        close(fileFd);
        fileFd = -1;
        currentFilePath = "";
        currentOperation = Inactive;
    }
}

void User::sendNextFilePart()
{
    if (currentOperation != Downloading)
    {
        Logger::LOG(ERROR, "User can't download");
        return;
    }

    std::vector<char> partData = vault->downloadFilePart(fileFd);

    MessageManager::getInstance()->addMessageOut(
                createMessage(FilePartMessage
                    {
                        fullFileSize,
                        static_cast<int>(partData.size()),
                        partData
                    },
                    client->sockfd));

    currentFileSize += partData.size();

    if (currentFileSize == fullFileSize)
    {
        close(fileFd);
        fileFd = -1;
        currentOperation = Inactive;
    }
}

void User::sendNextDirPart()
{
    if (currentOperation != Listing)
    {
        Logger::LOG(ERROR, "User can't list");
        return;
    }

    std::vector<DirEntry> entriesPart;
    int currentSize = 0;
    for (DirEntry entry : allDirEntries)
    {
        int entrySize = (entry.filePath.size() * sizeof(entry.filePath[0])) + sizeof(entry.modDate);
        if (currentSize + entrySize <= 4096)
        {
            entriesPart.push_back(entry);
            currentSize += entrySize;
        }
        else
            break;
    }

    MessageManager::getInstance()->addMessageOut(
                createMessage(DirPartMessage
                    {
                        static_cast<int>(allDirEntries.size()),
                        static_cast<int>(entriesPart.size()),
                        entriesPart
                    },
                    client->sockfd));

    entriesSent += entriesPart.size();

    if (entriesSent == static_cast<int>(allDirEntries.size()))
    {
        currentOperation = Inactive;
    }
}