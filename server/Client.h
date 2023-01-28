#pragma once

#include "Stage.h"

#include <string>

class client
{
private:
    int sockfd;
    std::string address;
    Stage currentStage;
};
