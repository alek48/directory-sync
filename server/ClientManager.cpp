#include "ClientManager.h"

#include <algorithm>

ClientManager* ClientManager::clientManager_ = nullptr;

ClientManager *ClientManager::getInstance()
{
    if(clientManager_==nullptr){
        clientManager_ = new ClientManager{};
    }
    return clientManager_;
}

void ClientManager::addClient(Client client)
{
    clients.push_back(client);
}

Client& ClientManager::getClientOnSock(int sockfd)
{
    return *getInter(sockfd);
}

std::vector<Client>::iterator ClientManager::getInter(int sockfd)
{
    return std::find_if(clients.begin(), clients.end(), 
        [sockfd](const Client& c){
            return c.sockfd == sockfd;
        });
}