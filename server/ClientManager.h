#pragma once

#include "Client.h"

#include <vector>

class ClientManager // singleton
{
protected:
    ClientManager() = default;
    static ClientManager* clientManager_;

public:
    ClientManager(ClientManager& other) = delete;
    void operator=(const ClientManager &) = delete;
    static ClientManager *getInstance();

    void addClient(int sockfd, std::string address);
    void removeClientOnSock(int sockfd);
    std::vector<Client>& getClients();
    Client& getClientOnSock(int sockfd);

private:
    std::vector<Client> clients;
    std::vector<Client>::iterator getInter(int sockfd);
};