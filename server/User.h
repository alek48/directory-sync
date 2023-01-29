#pragma once

#include "Client.h"

enum SyncOption
{
    Safe, Force
};

class User // client in vault
{
public:
    User(Client& client, SyncOption syncOption);

    const Client* client;
    SyncOption syncOption;
    bool privileged = false;
};