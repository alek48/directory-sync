#pragma once

#include <vector>
#include <string>

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


struct DirEntry
{
    std::string filePath;
    std::string fileName;
    int modDate;
};

struct DirPartMessage
{
    static std::vector<char> serialize(DirPartMessage dirPartMessage); // entries: filepath1:epoch_mod_date1;filepath2:epoch_mod_date2
    static DirPartMessage create(std::vector<char>& data);

    int numOfAllEntries;
    int numOfEntries;
    std::vector<DirEntry> entries;
};
