#include "Client.h"
#include "Stage.h"

Client::Client(int sockfd, std::string address, Stage currentStage) :
    sockfd {sockfd}, address {address}, currentStage {currentStage}
{
    
}

Client::Client(Client&& rhs) :
    sockfd {rhs.sockfd},
    address {rhs.address},
    currentStage {rhs.currentStage},
    user {std::move(rhs.user)}
{
    
}

Client& Client::operator=(Client&& rhs)
{
    this->sockfd = rhs.sockfd;
    this->address = std::move(rhs.address);
    this->currentStage = rhs.currentStage;
    this->user = std::move(rhs.user);

    return *this;
}

Client::~Client()
{

}

bool Client::isConnected() const
{
    return user.get() != nullptr;
}