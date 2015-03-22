#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>


//Class that makes the client socket handeling easier
class SocketClient
{
public:
    SocketClient();
    ~SocketClient();
    void Close();
    bool Connect();
    void DeleteMessage();
    std::string GetMessage();
    bool Init(std::string serveraddress, std::string portnumber, int newbuffersize);
    ssize_t Recv();
    ssize_t Send(std::string const& message);

private:
    int buffersize;
    bool connected;
    addrinfo host_info;
    addrinfo* host_info_list;
    std::string recvmessage;
    int socketfd;
    bool success_init;
};

#endif
