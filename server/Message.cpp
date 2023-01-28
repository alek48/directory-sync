#include "Message.h"

TextMessage TextMessage::create(std::vector<char>& data)
{
    TextMessage textMessage{};
    textMessage.text = std::string{&(data[0])};
    return textMessage;
}

CommandMessage CommandMessage::create(std::vector<char>& data)
{
    CommandMessage commandMessage{};
    // TODO:
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
