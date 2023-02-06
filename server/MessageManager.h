#pragma once

#include <vector>
#include <queue>

class Message;

void readMessageHeader(Message& message, char* data);
int readMessageData(Message& message, char* data, int dataLen);

class MessageManager // singleton
{
protected:
    MessageManager() = default;
    static MessageManager* messageManager_;

public:
    MessageManager(MessageManager& other) = delete;
    void operator=(const MessageManager &) = delete;
    static MessageManager *getInstance();

    void processMessageData(int sockfd, char* buffData, int buffLen);
    void processMessageQueues();
    void addMessageOut(Message message);
    void sendMessageToSock(std::vector<int> sockets);

    bool isMessageOutEmpty();

private:
    std::vector<Message> partialMessagesIn;
    std::queue<Message> messagesIn;
    std::vector<Message> messagesOut;

    int getMessageForSock(int sockfd);
};