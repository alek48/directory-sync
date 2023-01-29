#include "CommandProcessor.h"
#include "ClientManager.h"
#include "VaultManager.h"
#include "MessageManager.h"
#include "Logger.h"

void sendHelpMessage(Stage stage, int sockfd, bool wrongCommand = false)
{
    TextMessage message{""};
    if (wrongCommand)
        message.text += "Incorrect command\n";

    message.text += "Available commands for stage: " + stageName(stage) + '\n';

    for (const auto& command : getAvailableCommands(stage))
        message.text += command + '\n';

    MessageManager::getInstance()->addMessageOut(createMessage(message, sockfd));
}

void CommandProcessor::executeCommandMessage(Client& client, CommandMessage& commandMessage)
{
    Stage clientStage = client.currentStage;

    if (commandMessage.parts.size() == 1 && commandMessage.parts[0] == "help")
    {
        sendHelpMessage(clientStage, client.sockfd);
        return;
    }

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
                sendHelpMessage(clientStage, client.sockfd, true);
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
            else
            {
                sendHelpMessage(clientStage, client.sockfd, true);
            }
        }
        else if (commandMessage.parts.size() == 3)
        {
            std::string command = commandMessage.parts[0];
            std::string vaultName = commandMessage.parts[1];
            std::string type = commandMessage.parts[2];

            if (command == "sync")
            {
                // does vault exist?
                if (!VaultManager::getInstance()->isCreated(vaultName))
                {
                    MessageManager::getInstance()->addMessageOut(
                        createMessage(TextMessage{"Vault " + vaultName + " does not exist"}, client.sockfd));
                    return;
                }

                Vault& vault = VaultManager::getInstance()->getVault(vaultName);
                
                bool success = vault.sync(client, type);

                if (!success)
                {
                    MessageManager::getInstance()->addMessageOut(
                        createMessage(TextMessage{"Could not start sync with vault: " + vaultName}, client.sockfd));
                }
                else
                {
                    MessageManager::getInstance()->addMessageOut(
                        createMessage(TextMessage{"Syncing"}, client.sockfd));
                }

                if (type != "safe" || type != "force")
                {
                    sendHelpMessage(clientStage, client.sockfd, true);
                }
            }
            else
            {
                sendHelpMessage(clientStage, client.sockfd, true);
            }
        }
        else
        {
            sendHelpMessage(clientStage, client.sockfd, true);
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
                    sendHelpMessage(clientStage, client.sockfd, true);
                }
            }
            else
            {
                sendHelpMessage(clientStage, client.sockfd, true);
            }
        }
        else if (commandMessage.parts[0].size() == 2)
        {
            if (commandMessage.parts[0] == "list" && 
                commandMessage.parts[1] == "entries")
            {
                // std::vector<DirEntry> allEntries = 
                // MessageManager::getInstance()->addMessageOut(
                //     createMessage(DirPartMessage{"Could not start sync with vault: " + vaultName}, client.sockfd));
            }
            else
            {
                sendHelpMessage(clientStage, client.sockfd, true);
            }
        }
        else
        {
            sendHelpMessage(clientStage, client.sockfd, true);
        }
    }
    else
    {
        Logger::LOG(ERROR, "Unknown client stage");
    }
}
