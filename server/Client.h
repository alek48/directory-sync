#pragma once

#include "Stage.h"
#include "User.h"

#include <string>
#include <memory>

class Client
{
public:
    Client(int sockfd, std::string address = "", Stage currentStage = Connected);
    Client(Client&& rhs);

    ~Client();

    Client& operator=(Client&& rhs);

    bool isConnected() const;

    int sockfd;
    std::string address;
    Stage currentStage;
    std::unique_ptr<User> user {};
};
