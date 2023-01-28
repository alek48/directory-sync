#pragma once

#include "Client.h"

class ClientManager // singleton
{
protected:
    ClientManager() : clients{} {}
    static ClientManager* clientManager_;

public:
    ClientManager(ClientManager& other) = delete;
    void operator=(const ClientManager &) = delete;
    static ClientManager *getInstance();

    void addClient(Client client);
    void removeClientOnSock(int sockfd);
    Client& getClientOnSock(int sockfd);

private:
    std::vector<Client> clients;
    std::vector<Client>::iterator getInter(int sockfd);
};