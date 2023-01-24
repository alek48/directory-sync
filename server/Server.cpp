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
#include <poll.h>
#include <sys/fcntl.h>

#include <vector>

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
    struct sockaddr_in *ipv4 = (sockaddr_in*)info->ai_addr;
    void *addr = &(ipv4->sin_addr);
    inet_ntop(ipv4->sin_family, addr, ipstr, sizeof(ipstr));
    return std::string{ipstr};
}

Server::Server(std::string port)
    : port {port}
{
    Logger::LOG(INFO, "server created");
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

    if (fcntl(listenSockfd, F_SETFL, O_NONBLOCK) == -1)
    {
        Logger::LOG(ERROR, "server: failed to set non-blocking socket");
        throw InitException{};
    }

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
    std::vector<pollfd> pfds{};

    pollfd listener {listenSockfd, POLLIN};
    pfds.push_back(listener);

    while (true)
    {
        int poll_count = poll(&pfds[0], pfds.size(), -1);

        if (poll_count == -1)
        {
            Logger::LOG(ERROR, "poll");
            // throw
        }

        for (int i = 0; i < pfds.size(); i++)
        {
            if (pfds[i].revents & POLLIN)
            {
                // ready to read
                if (pfds[i].fd == listenSockfd)
                {
                    // new connection
                    sockaddr_storage remoteaddr;
                    socklen_t addrlen = sizeof(remoteaddr);
                    int newfd = accept(listenSockfd,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);
                    
                    if (remoteaddr.ss_family != AF_INET)
                    {
                        Logger::LOG(ERROR, "only IPv4");
                        close(newfd);
                        // throw
                    }

                    char remoteIP[INET_ADDRSTRLEN];
                    std::string newCliendAddress = inet_ntop(AF_INET, &remoteaddr,
                        remoteIP, INET_ADDRSTRLEN);

                    Logger::LOG(INFO, "new connection from: " + std::string{remoteIP});
                }
                else
                {
                    // already connected client
                    // read message
                }
            }
            else if (pfds[i].revents & POLLOUT)
            {
                // ready to write
            }
        }
    }
}