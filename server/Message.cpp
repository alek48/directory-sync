#include "Message.h"
#include "Vault.h"
#include "helpers.h"

#include <vector>
#include <string>

TextMessage TextMessage::create(std::vector<char>& data)
{
    TextMessage textMessage{};
    for (char c : data)
        textMessage.text += c;
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

std::vector<char> CommandMessage::serialize(CommandMessage commandMessage)
{
    std::vector<char> data;
    // TODO:
    // but server doesn't need it
    return data;
}

FilePartMessage FilePartMessage::create(std::vector<char>& data)
{
    FilePartMessage filePartMessage{};
    // TODO:
    return filePartMessage;
}

std::vector<char> FilePartMessage::serialize(FilePartMessage filePartMessage)
{
    std::vector<char> data;
    // TODO:
    return data;
}

DirPartMessage DirPartMessage::create(std::vector<char>& data)
{
    DirPartMessage dirPartMessage{};
    // TODO:
    return dirPartMessage;
}

std::vector<char> DirPartMessage::serialize(DirPartMessage dirPartMessage)
{
    std::vector<char> data;

    appendIntToNetworkData(dirPartMessage.numOfAllEntries, data);
    appendIntToNetworkData(dirPartMessage.numOfEntries, data);

    for (const DirEntry& entry : dirPartMessage.entries)
    {
        appendStringToNetworkData(entry.filePath, data);
        data.push_back(';');
        appendIntToNetworkData(entry.modDate, data);
        data.push_back(' '); // entry end
    }
    return data;
}

Message createMessage(TextMessage textMessage, int sockfd)
{
    std::vector<char> messageData = TextMessage::serialize(textMessage);
    return Message{static_cast<int>(messageData.size()), 
        Text, sockfd, messageData};
}

Message createMessage(CommandMessage commandMessage, int sockfd)
{
    std::vector<char> messageData = CommandMessage::serialize(commandMessage);
    return Message{static_cast<int>(messageData.size()), 
        Command, sockfd, messageData};
}

Message createMessage(FilePartMessage filePartMessage, int sockfd)
{
    std::vector<char> messageData = FilePartMessage::serialize(filePartMessage);
    return Message{static_cast<int>(messageData.size()), 
        FilePart, sockfd, messageData};
}

Message createMessage(DirPartMessage dirPartMessage, int sockfd)
{
    std::vector<char> messageData = DirPartMessage::serialize(dirPartMessage);
    return Message{static_cast<int>(messageData.size()), 
        DirPart, sockfd, messageData};
}