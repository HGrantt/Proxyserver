#include "socketserver.h"

SocketServer::SocketServer() :
    buffersize(0),
    recvmessage(""),
    socketfd(0)
{}

SocketServer::~SocketServer()
{
    Close();
}

void SocketServer::Close()
{
    if (socketfd != 0)
    {
	close(socketfd);
    }
}

void SocketServer::DeleteMessage()
{
    recvmessage = "";
}

std::string SocketServer::GetMessage()
{
    return recvmessage;
}

void SocketServer::Init(int newsocketfd, int newbuffersize = 1024)
{
    buffersize = newbuffersize;
    socketfd = newsocketfd;
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
}

ssize_t SocketServer::Recv()
{
    ssize_t bytes_recieved = 0;
    ssize_t total = 0;
    if (socketfd != 0)
    {
        char incoming_data_buffer[buffersize];
        memset(incoming_data_buffer, 0, sizeof(incoming_data_buffer));
  
	while ((bytes_recieved = recv(socketfd, incoming_data_buffer, buffersize, 0)) > 0)
	{
	    recvmessage.append(incoming_data_buffer, bytes_recieved);
	    total += bytes_recieved;
	}
    }
    else
    {
	std::cout << "Error in SocketServer::Recv: must call Init first" << std::endl;
    }
    return total;
}

ssize_t SocketServer::Send(std::string const& message)
{
    if (socketfd != 0 && message.length() > 0)
    {
	int len = message.length();
	return send(socketfd, message.c_str(), len, 0);
    }
    else
    {
	std::cout << "Error in SocketServer::Send: must call Init first" << std::endl;
	return 0;
    }
}
