#pragma once

#include <vector>
#include <string>

class Vault;
struct DirEntry;

constexpr int MAX_MESSAGE_SIZE = 4096;

enum MessageType
{
    Text, Command, FilePart, DirPart
};

struct Message
{
    int len;
    MessageType type;
    int sock;
    std::vector<char> data;
    int currentLen = 0;
};

struct TextMessage
{
    static std::vector<char> serialize(TextMessage textMessage);
    static TextMessage create(std::vector<char>& data);

    std::string text;
};

struct CommandMessage
{
    static std::vector<char> serialize(CommandMessage commandMessage);
    static CommandMessage create(std::vector<char>& data);

    std::vector<std::string> parts;
};

struct FilePartMessage
{
    static std::vector<char> serialize(FilePartMessage filePartMessage);
    static FilePartMessage create(std::vector<char>& data);

    int fullSize;
    int partSize;
    std::vector<char> partData;
};

struct DirPartMessage
{
    static std::vector<char> serialize(DirPartMessage dirPartMessage); // entries: filepath1:epoch_mod_date1;filepath2:epoch_mod_date2
    static DirPartMessage create(std::vector<char>& data);

    int numOfAllEntries;
    int numOfEntries;
    std::vector<DirEntry> entries;
};

Message createMessage(TextMessage textMessage, int sockfd);
Message createMessage(CommandMessage commandMessage, int sockfd);
Message createMessage(FilePartMessage filePartMessage, int sockfd);
Message createMessage(DirPartMessage dirPartMessage, int sockfd);