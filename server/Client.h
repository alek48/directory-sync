#pragma once

#include "Stage.h"

#include <string>

class Client
{
public:
    Client(int sockfd, std::string address = "");

    int sockfd;
    std::string address;
    Stage currentStage;
};
