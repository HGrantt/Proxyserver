#include "socketclient.h"

SocketClient::SocketClient() :
    buffersize(0),
    connected(false),
    host_info(),
    host_info_list(),
    recvmessage(""),
    socketfd(0),
    success_init(false)
{}

SocketClient::~SocketClient()
{
    Close();
}

void SocketClient::Close()
{
    if (success_init)
    {
	success_init = false;
	freeaddrinfo(host_info_list);
	close(socketfd);
    }
}

bool SocketClient::Connect()
{
    if (success_init)
    {
	int status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1)
	{
	    std::cout << "Error in SocketClient::Connect: connect" << std::endl;
	    return false;
	}
	connected = true;
	return true;
    }
    else
    {
	std::cout << "Error in SocketClient::Connect; must call Init first" << std::endl;
	return false;
    }
}

void SocketClient::DeleteMessage()
{
    recvmessage = "";
}

std::string SocketClient::GetMessage()
{
    return recvmessage;
}

bool SocketClient::Init(std::string serveraddress, std::string portnumber, int newbuffersize = 1024)
{
    memset(&host_info, 0, sizeof host_info);
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(serveraddress.c_str(), portnumber.c_str(), &host_info, &host_info_list) != 0)
    {
	std::cout << "Error in SocketClient::Init: getaddrinfo" << std::endl;
	return false;
    }
    
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)
    {
	std::cout << "Error in SocketClient::Init: socket" << std::endl;
	return false;
    }
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
    success_init = true;
    buffersize = newbuffersize;
    return true;
}

ssize_t SocketClient::Recv()
{
    ssize_t bytes_recieved = 0;
    ssize_t total = 0;
    if (connected)
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
	std::cout << "Error in SocketClient::Recv: must call Connect first" << std::endl;
    }
    return total;
}

ssize_t SocketClient::Send(std::string const& message)
{
    if (connected && message.length() > 0)
    {
	int len = message.length();
	return send(socketfd, message.c_str(), len, 0);
    }
    else
    {
	std::cout << "Error in SocketClient::Send: must call Connect first" << std::endl;
	return 0;
    }
}
