#ifndef SOCKETLISTEN_H
#define SOCKETLISTEN_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>


//Class that makes client socket listening easier
class SocketListen
{
public:
    SocketListen();
    ~SocketListen();
    int Accept();
    bool Bind();
    void Close();
    bool Init(std::string portn);
    bool Listen();
    
private:
    addrinfo host_info;
    addrinfo* host_info_list;
    std::string portnumber;
    int socketfd;
    bool success_bind;
    bool success_init;
    bool success_listen;
};

#endif
