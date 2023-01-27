#pragma once

#include <string>

#include "MessageManager.h"

class Server
{
public:
    Server(std::string port = "1515");

    void init();
    void run();

private:

    int listenSockfd;
    std::string port;
    int backlog = 10;
    MessageManager messageManager;
};