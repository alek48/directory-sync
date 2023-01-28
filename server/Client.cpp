#include "Client.h"

Client::Client(int sockfd, std::string address) :
    sockfd {sockfd}, address {address}, currentStage {Connected}
{

}