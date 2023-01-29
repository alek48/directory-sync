#include "Client.h"
#include "Stage.h"

Client::Client(int sockfd, std::string address) :
    sockfd {sockfd}, address {address}, currentStage {Connected}
{

}

Client::~Client()
{
    
}