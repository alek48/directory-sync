class Client;
class CommandMessage;

class CommandProcessor
{
public:
    static void executeCommandMessage(Client& client, CommandMessage& commandMessage);
};
