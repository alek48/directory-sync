#pragma once

#include "Message.h"
#include "Logger.h"

#include <netinet/in.h>

#include <vector>
#include <queue>

void readHeader(Message& message, char* data);
int readMessageData(Message& message, char* data, int dataLen);

class MessageManager
{
public:
    void processMessageData(int sockfd, char* buffData, int buffLen);

private:
    std::vector<Message> partialMessagesIn;
    std::queue<Message> messagesIn;
    std::queue<Message> messagesOut;

    int getMessageForSock(int sockfd);
};