#include "Message.h"

TextMessage TextMessage::create(std::vector<char>& data)
{
    TextMessage textMessage{};
    textMessage.text = std::string{&(data[0])};
    return textMessage;
}

std::vector<char> TextMessage::serialize(TextMessage textMessage)
{
    std::vector<char> data;
    for (char c : textMessage.text)
    {
        data.push_back(c);
    }
    return data;
}

CommandMessage CommandMessage::create(std::vector<char>& data)
{
    CommandMessage commandMessage{};
    std::string part;
    for (char c: data)
    {
        if (c == ' ')
        {
            commandMessage.parts.push_back(std::move(part));
            part = std::string{};
        }
        else
        {
            part += c;
        }
    }
    commandMessage.parts.push_back(std::move(part));
    return commandMessage;
}

FilePartMessage FilePartMessage::create(std::vector<char>& data)
{
    FilePartMessage filePartMessage{};
    // TODO:
    return filePartMessage;
}

DirPartMessage DirPartMessage::create(std::vector<char>& data)
{
    DirPartMessage dirPartMessage{};
    // TODO:
    return dirPartMessage;
}
