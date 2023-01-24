#include "Server.h"
#include "Logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

struct InitException : std::exception
{
    const char* what() const noexcept
    {
        return "Failure when initializing server";
    }
};

std::string addrinfoToString(addrinfo* info)
{
    char ipstr[INET_ADDRSTRLEN];
    struct sockaddr_in *ipv4 = (struct sockaddr_in*)info->ai_addr;
    void *addr = &(ipv4->sin_addr);
    inet_ntop(info->ai_family, addr, ipstr, sizeof(ipstr));
    return std::string{ipstr};
}

Server::Server(std::string port)
    : port {port}
{
    Logger::LOG(INFO, "Server created");
}

void Server::init()
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use default IP

    int rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);
    if (rv != 0)
    {
        Logger::LOG(ERROR, "getaddrinfo failed!");
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        throw InitException{};
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET6)
        {
            Logger::LOG(INFO, "skipping IPv6");
            continue;
        }

        if ((listenSockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1)
        {
            Logger::LOG(ERROR, "server: socket");
            continue;
        }

        int valTrue = 1;
        if (setsockopt(listenSockfd, SOL_SOCKET, SO_REUSEADDR, &valTrue,
                sizeof(int)) == -1)
        {
            Logger::LOG(ERROR, "setsockopt");
            throw InitException{};
        }

        if (bind(listenSockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listenSockfd);
            Logger::LOG(ERROR, "server: bind");
            continue;
        }

        break;
    }

    Logger::LOG(INFO, std::string{"server: "} +
        addrinfoToString(servinfo) + std::string{":"} + port);

    freeaddrinfo(servinfo);

    if (p == NULL) 
    {
        Logger::LOG(ERROR, "server: failed to bind");
        throw InitException{};
    }

    if (listen(listenSockfd, backlog) == -1)
    {
        Logger::LOG(ERROR, "server: listen");
        throw InitException{};
    }



    Logger::LOG(INFO, "server: listening...");
}

void Server::run()
{
    
}