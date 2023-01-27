#include "MessageManager.h"

void readHeader(Message& message, char* data)
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
        message.data[message.currentLen] = *data;
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
            readHeader(message, buffData);
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

int MessageManager::getMessageForSock(int sockfd)
    {
        int num = 0;
        int idx = 0;
        for (int i = 0; i < partialMessagesIn.size(); i++)
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