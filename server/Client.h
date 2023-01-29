#pragma once

#include "Stage.h"

#include <string>

class User;

class Client
{
public:
    Client(int sockfd, std::string address = "");
    ~Client();

    User* user = nullptr;
    int sockfd;
    std::string address;
    Stage currentStage;
};
