#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>


//Class that makes the socket server handeling simpler
class SocketServer
{
public:
    SocketServer();
    ~SocketServer();
    void Close();
    void DeleteMessage();
    std::string GetMessage();
    void Init(int newsocketfd, int newbuffersize);
    ssize_t Recv();
    ssize_t Send(std::string const& message);

private:
    int buffersize;
    std::string recvmessage;
    int socketfd;
};

#endif
