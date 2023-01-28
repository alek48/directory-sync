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
    int currentLen = 0;
    std::vector<char> data;
};

struct TextMessage
{
    static std::vector<char> serialize();
    static TextMessage create(std::vector<char>& data);

    std::string text;
};

struct CommandMessage
{
    static std::vector<char> serialize();
    static CommandMessage create(std::vector<char>& data);

    std::vector<std::string> parts;
};

struct FilePartMessage
{
    static std::vector<char> serialize();
    static FilePartMessage create(std::vector<char>& data);
};

struct DirPartMessage
{
    static std::vector<char> serialize();
    static DirPartMessage create(std::vector<char>& data);
};
