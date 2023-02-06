#include "ClientManager.h"
#include "Client.h"

#include <vector>
#include <algorithm>

ClientManager* ClientManager::clientManager_ = nullptr;

ClientManager *ClientManager::getInstance()
{
    if(clientManager_==nullptr){
        clientManager_ = new ClientManager{};
    }
    return clientManager_;
}

void ClientManager::addClient(int sockfd, std::string address)
{
    clients.emplace_back(sockfd, address);
}

Client& ClientManager::getClientOnSock(int sockfd)
{
    return *getInter(sockfd);
}

void ClientManager::removeClientOnSock(int sockfd)
{
    clients.erase(getInter(sockfd));
}

std::vector<Client>& ClientManager::getClients()
{
    return clients;
}

std::vector<Client>::iterator ClientManager::getInter(int sockfd)
{
    return std::find_if(clients.begin(), clients.end(), 
        [sockfd](const Client& c){
            return c.sockfd == sockfd;
        });
}