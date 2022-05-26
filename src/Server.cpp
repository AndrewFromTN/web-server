#include "Server.h"
#include "ThreadFunctions.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

bool open_welcome(std::string ip_address, int port, int& welcome_socket)
{
	welcome_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (welcome_socket == -1)
	{
		puts("Could not open a welcoming socket. errno: " + errno);
		return false;
	}

	int opt = 1;
	if (setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		puts("Could not set welcoming socket properties.");
		return false;
	} 

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip_address.c_str());
	address.sin_port = htons(port);

	if (bind(welcome_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		puts("Failed to bind the welcoming socket.");
		return false;
	}

	if (listen(welcome_socket, 3) == -1)
	{
		puts("Failed to start listening.");
		return false;
	}
	
	return true;
}

