#include <iostream>
#include <string>
#include <ctime>
#include <stdio.h>
#include <unistd.h>

#include "socketlisten.h"
#include "socketserver.h"
#include "socketclient.h"
#include "messagevalidator.h"

using namespace std;


//Handles one connection between the server and the client.
void HandleBridge(int socketfd, int counter)
{
    //Initiation of word control and the proxy to client connection
    MessageValidator msgvalid;
    SocketServer server;

    server.Init(socketfd, 1024);
    while (server.Recv() < 0) {}

    msgvalid.AddInvalidWord("britney spears");
    msgvalid.AddInvalidWord("paris hilton");
    msgvalid.AddInvalidWord("spongebob");
    msgvalid.AddInvalidWord("norrköping");

    //If bad URL redirect
    if (!msgvalid.ValidRequest(server.GetMessage()))
    {
	server.Send("HTTP/1.1 302 Found\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\n\n");
	server.Close();
	return;
    }

    //Cleaning the request for easier use.
    string crlf = {0x0d, 0x0a};
    string totmessage = msgvalid.GetNewRequest(server.GetMessage());
    totmessage = msgvalid.FixRequestHeader(totmessage);
    size_t beginpos = totmessage.find("ost:");
    size_t endpos = totmessage.find(crlf, beginpos);
    string hostname = totmessage.substr(beginpos+5, endpos-beginpos-5);

    cout << counter << ": " << msgvalid.GetField(totmessage, "GET") << flush;

    //Initiation of the proxy to server connection
    SocketClient client;
    client.Init(hostname, "80", 1024);
    client.Connect();

    ssize_t bytes = -1;

    //Send recieve loop
    while (bytes != 0)
    {
	client.Send(msgvalid.FixRequestHeader(msgvalid.GetNewRequest(server.GetMessage())));
	if ((bytes = client.Recv()) == 0)
	{
	    break;
	}

        //If bad content i webpage, redirect instead. 
	if (!msgvalid.ValidResponse(client.GetMessage()))
	{
	    server.Send("HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\n\r\n");
	    break;
	}

        //cout << counter << ": " << client.GetMessage() << endl;
        
	server.Send(client.GetMessage());
	server.DeleteMessage();
	client.DeleteMessage();
 	if ((bytes = server.Recv()) == 0)
	{
	    break;
	}
        //cout << counter << ": " << server.GetMessage() << endl;
    }

    //End of connection
    client.Close();
    server.Close();
}


//Arguments for main() = Proxy portnumber. 
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "USAGE: <program> <portnumber>" << endl;
        return 1;
    }
    SocketListen listen;
    listen.Init(argv[1]);
    listen.Bind();

    cout << "Listening for connections on port: " << argv[1] << endl;
    
    pid_t pid;
    bool running = true;
    int newsocketfd = -1;

    int counter = 0;

    //Listening loop
    while (running)
    {
	listen.Listen();
	newsocketfd = listen.Accept();
	if (newsocketfd != -1)
	{
	    ++counter;
	    cout << "Child #" << counter << " created" << endl;
	    pid = fork();
	    if (pid == 0)
	    {
		running = false;
		HandleBridge(newsocketfd, counter);
	    }
	    else if (pid > 0)
	    {
	        close(newsocketfd);
	    }
	}
	
    }

    //Closing the connection
    if (pid > 0)
    {
	listen.Close();
	cout << "Main proccess terminated" << endl;
    }
    cout << "Child #" << counter << " killed" << endl;
    return 0;
}
