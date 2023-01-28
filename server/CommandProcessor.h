#include "Message.h"
#include "Client.h"

class CommandProcessor
{
public:
    static Message executeCommandMessage(Client& client, CommandMessage& commandMessage);
};
