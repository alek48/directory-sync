#include "CommandProcessor.h"
#include "ClientManager.h"

Message CommandProcessor::executeCommandMessage(Client& client, CommandMessage& commandMessage)
{
    TextMessage response{};
    Message responseMessage{};

    Stage clientStage = client.currentStage;

    if (clientStage == Connected)
    {
        // list [n] command
        if (commandMessage.parts[0] == "list")
        {
            
        }
        else if (commandMessage.parts[0] == "create")
        {
            
        }
        else if (commandMessage.parts[0] == "sync")
        {
            
        }
    }
    else if (clientStage == Syncing)
    {

    }
    else
    {

    }

    std::vector<char> responseData = TextMessage::serialize(response);
    return Message{static_cast<int>(responseData.size()), Text, client.sockfd, responseData};
}