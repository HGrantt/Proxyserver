#include "socketlisten.h"

SocketListen::SocketListen() :
    host_info(),
    host_info_list(),
    portnumber(""),
    socketfd(),
    success_bind(false),
    success_init(false),
    success_listen(false)
{}

SocketListen::~SocketListen()
{
    close(socketfd);
}

int SocketListen::Accept()
{
    if (success_listen)
    {
	sockaddr_storage their_addr;
	socklen_t addr_size;
	return accept(socketfd, (sockaddr*)&their_addr, &addr_size);
    }
    else
    {
	std::cout << "Error in SocketListen::Accept: must call Listen first" << std::endl;
	return 0;
    }
}

bool SocketListen::Bind()
{
    if (success_init)
    {
	int status;
	int yes = 1;
	status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1)
	{
	    std::cout << "Error in SocketListen::Bind: bind" << std::endl;
	    return false;
	}
	success_bind = true;
	return true;
    }
    return false;
}

void SocketListen::Close()
{
    if (success_init)
    {
	success_bind = false;
	success_init = false;
	success_listen = false;
	close(socketfd);
	freeaddrinfo(host_info_list);
	portnumber = "";
    }
}

bool SocketListen::Init(std::string portn)
{
    memset(&host_info, 0, sizeof host_info);
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(nullptr, portn.c_str(), &host_info, &host_info_list) != 0)
    {
	std::cout << "Error in SocketListen::Init: getaddrinfo" << std::endl;
	return false;
    }
    
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)
    {
	std::cout << "Error in SocketListen::Init: socket" << std::endl;
	return false;
    }
    success_init = true;
    portnumber = portn;
    return true;
}

bool SocketListen::Listen()
{
    if (success_bind)
    {
	int status =  listen(socketfd, 10);
	if (status == -1)
	{
	    std::cout << "Error in SocketListen::Listen: listen" << std::endl;
	    return false;
	}
	success_listen = true;
	return true;
    }
    else
    {
	std::cout << "Error in SocketListen::Listen: must call Bind first" << std::endl;
	return false;
    }
}

