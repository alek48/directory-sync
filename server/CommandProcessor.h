#include "Message.h"
#include "Client.h"

class CommandProcessor
{
public:
    static void executeCommandMessage(Client& client, CommandMessage& commandMessage);
};
