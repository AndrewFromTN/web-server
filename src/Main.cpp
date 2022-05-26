#include "Server.h"
#include "ThreadFunctions.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		puts("Usage: ./WebServer.out <ip> <port>");
		return -1;
	}
	
	if (atoi(argv[2]) <= 1024)
	{
		puts("Warning: Not and ephemoral port. Choose a port above 1024 or run as root (not recommended).");
	}

	int welcome;
	if (!open_welcome(argv[1], atoi(argv[2]), welcome))
	{
		puts("Failed to open welcome. Quiting...");
		return -1;
	}

	int c = sizeof(struct sockaddr_in);
	while (true)
	{
		struct sockaddr_in client;
		int new_socket = accept(welcome, (struct sockaddr *)&client, (socklen_t*)&c);
		if (new_socket != -1)
		{
			puts("Accepted a connection.");

			pthread_t client_thread;

			struct SocketListenerData thread_data;
			thread_data.client_socket = &new_socket;
			pthread_create(&client_thread, NULL, thread_callback_socket_listener, 
				      (void*)&thread_data);

			pthread_detach(client_thread);			
		}
		else
		{
			puts("Failed to accept a connection. errno: " + errno);
		}		
	}

	pthread_exit(NULL);

	return 0;
}
