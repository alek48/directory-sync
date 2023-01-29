#include "MessageManager.h"
#include "CommandProcessor.h"
#include "ClientManager.h"
#include "Message.h"
#include "Logger.h"

#include <netinet/in.h>

#include <algorithm>

MessageManager* MessageManager::messageManager_ = nullptr;

MessageManager *MessageManager::getInstance()
{
    if(messageManager_==nullptr){
        messageManager_ = new MessageManager{};
    }
    return messageManager_;
}

void readMessageHeader(Message& message, char* data)
{
    // cast char* to int* and read bytes as int
    message.len = ntohl(*reinterpret_cast<int*>(data));
    // move by 4 bytes
    message.type = static_cast<MessageType>(ntohl(*reinterpret_cast<int*>(data+4)));
}

int readMessageData(Message& message, char* data, int dataLen)
{
    int remaining = message.len - message.currentLen;
    int savedData = 0;
    for (int i = 0; i < remaining && i < dataLen; i++)
    {
        message.data.push_back(*data);
        ++message.currentLen;
        ++data;
        ++savedData;
    }

    return savedData;
}

void MessageManager::processMessageData(int sockfd, char* buffData, int buffLen)
{
    int messageIdx = getMessageForSock(sockfd);

    if (messageIdx == -1)
    {
            // new message
        if (buffLen < 8)
        {
            // can't read header
            Logger::LOG(ERROR, "Message too short");
            return;
        }
        else
        {
            Message message{};
            message.sock = sockfd;
            readMessageHeader(message, buffData);
            buffData += 8; // move by 8 bytes
            buffLen -= 8;

            partialMessagesIn.push_back(std::move(message));
            messageIdx = partialMessagesIn.size() - 1;
        }
    }

    Message& message = partialMessagesIn[messageIdx];
    int savedData = readMessageData(message, buffData, buffLen);
    
    if (message.currentLen == message.len) // have full message
    {
        messagesIn.push(std::move(partialMessagesIn[messageIdx]));
        partialMessagesIn.erase(std::next(partialMessagesIn.begin(), messageIdx));
    }

    if (savedData < buffLen) // leftovers in buffer
    {
        // recursion
        processMessageData(sockfd, buffData + savedData, buffLen - savedData);
    }
}

void MessageManager::processMessageQueue()
{
    while (!messagesIn.empty()) // input messages
    {
        Message& message = messagesIn.front();
        Client& client = ClientManager::getInstance()->getClientOnSock(message.sock);

        if (message.type == Text)
        {
            TextMessage textMessage = TextMessage::create(message.data);
            Logger::LOG(INFO, "Got message from client: " + textMessage.text);
        }
        else if (message.type == Command)
        {
            CommandMessage commandMessage = CommandMessage::create(message.data);
            CommandProcessor::executeCommandMessage(client, commandMessage);
        }
        else if (message.type == FilePart)
        {
            FilePartMessage filePartMessage = FilePartMessage::create(message.data);
        }
        else if (message.type == DirPart)
        {
            Logger::LOG(ERROR, "Server received dir part from client");
        }
        else
        {
            Logger::LOG(ERROR, "Wrong message type");
        }

        messagesIn.pop();
    }
}

void appendIntToNetworkData(int val, std::vector<char>& data)
{
    int nVal = htonl(val);
    char* networkVal = reinterpret_cast<char*>(&nVal);
    for (int i = 0; i < 4; i++)
        data.push_back(networkVal[i]);
}

void sendAll(const Message& message)
{
    std::vector<char> headerAndData;

    appendIntToNetworkData(message.data.size(), headerAndData);
    appendIntToNetworkData(message.type, headerAndData);

    for (char c : message.data)
    {
        headerAndData.push_back(c);
    }

    int len = headerAndData.size();
    const char* dataStart = &(headerAndData[0]);
    int sockfd = message.sock;
    int total = 0;
    int bytesleft = len;

    while (total < len)
    {
        int n = send(sockfd, dataStart + total, bytesleft, 0);
        if (n == -1)
        {
            Logger::LOG(ERROR, "Failed to send message");
            break;
        }
        total += n;
        bytesleft -= n;
    }
}

void MessageManager::sendMessageToSock(std::vector<int> sockets)
{
    if (messagesOut.size() != 0)
    {
        for (unsigned i = 0; i < messagesOut.size(); i++)
        {
            int sockfd = messagesOut[i].sock;
            if (sockfd == *std::find_if(sockets.begin(), sockets.end(),
                [sockfd](int sock){return sockfd == sock;})) // find if sockfd is in ready sockets
            {
                sendAll(messagesOut[i]);
                messagesOut[i].len = -1; // mark as ready to delete
            }
        }

        // remove marked messages
        messagesOut.erase(std::remove_if(messagesOut.begin(), messagesOut.end(), 
            [](const Message& m){return m.len == -1;}), messagesOut.end());
    }
}

bool MessageManager::isMessageOutEmpty()
{
    return messagesOut.empty();
}

void MessageManager::addMessageOut(Message message)
{
    messagesOut.push_back(message);
}

int MessageManager::getMessageForSock(int sockfd)
{
    int num = 0;
    int idx = 0;
    for (unsigned i = 0; i < partialMessagesIn.size(); i++)
    {
        if (partialMessagesIn[i].sock == sockfd)
        {
            ++num;
            idx = i; 
        }
    }

    if (num == 0)
        return -1;
    else if (num == 1)
        return idx;
    else
    {
        Logger::LOG(ERROR, "More than one message at a time for single client");
        throw std::exception{};
    }
}