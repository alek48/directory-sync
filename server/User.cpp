#include "User.h"

User::User(Client& client, SyncOption syncOption) :
        client {&client}, syncOption {syncOption}
{
    client.user = this; // client and user point to each other
}