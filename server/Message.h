#pragma once

#include <array>

constexpr int MAX_MESSAGE_SIZE = 4096;

enum MessageType
{
    Command, Request, FilePart
};

struct Message
{
    int len;
    MessageType type;
    int sock;
    int currentLen = 0;
    std::array<char, MAX_MESSAGE_SIZE> data;
};