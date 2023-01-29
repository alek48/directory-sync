#include "CommandProcessor.h"
#include "ClientManager.h"
#include "VaultManager.h"
#include "MessageManager.h"
#include "Logger.h"

void CommandProcessor::executeCommandMessage(Client& client, CommandMessage& commandMessage)
{
    Stage clientStage = client.currentStage;

    if (clientStage == Connected)
    {
        if (commandMessage.parts.size() == 1)
        {
            if (commandMessage.parts[0] == "list")
            {
                std::vector<std::string> vaults = VaultManager::getInstance()->listAllVaults();
                TextMessage response{};
                for (const std::string& vault : vaults)
                {
                    response.text += vault + "\n";
                }
                MessageManager::getInstance()->addMessageOut(createMessage(response, client.sockfd));
            }
            else
            {
                // wrong
            }
        }
        else if (commandMessage.parts.size() == 2)
        {
            if (commandMessage.parts[0] == "list")
            {
                std::string page = commandMessage.parts[1];
            }
            else if (commandMessage.parts[0] == "create")
            {
                std::string vaultName = commandMessage.parts[1];
                bool success = VaultManager::getInstance()->createVault(vaultName);
                TextMessage response{};
                if (success)
                    response.text = "Vault " + vaultName + " created";
                else
                    response.text = "Vault " + vaultName + " could not be created";

                MessageManager::getInstance()->addMessageOut(createMessage(response, client.sockfd));
            }
        }
        else if (commandMessage.parts.size() == 3)
        {
            std::string command = commandMessage.parts[0];
            std::string vaultName = commandMessage.parts[1];
            std::string type = commandMessage.parts[2];

            if (command == "sync")
            {
                if (type == "upload")
                {

                }
                else if (type == "download")
                {

                }
                else
                {
                    // wrong type
                }
            }
            else
            {
                // wrong
            }
        }
        else
        {
            // wrong
        }
    }
    else if (clientStage == Syncing)
    {
        if (commandMessage.parts[0].size() == 3)
        {
            if (commandMessage.parts[0] == "request")
            {
                std::string filepath = commandMessage.parts[2];
                if (commandMessage.parts[1] == "download")
                {
                    
                }
                else if (commandMessage.parts[1] == "upload")
                {
                    
                }
                else
                {
                    // wrong option
                }
            }
            else
            {
                // wrong command
            }
        }
        else if (commandMessage.parts[0].size() == 2)
        {
            if (commandMessage.parts[0] == "list" && 
                commandMessage.parts[0] == "entries")
            {
                
            }
            else
            {
                // wrong command
            }
        }
        else
        {
            // wrong command
        }
    }
    else
    {
        Logger::LOG(ERROR, "Unknown client stage");
    }
}
