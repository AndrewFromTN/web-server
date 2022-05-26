#pragma once

#include <pthread.h>

/* 
   I dont't necessarily need full blown struct for 1 param,
   but if I need more params in the future it will be easier.
*/
struct SocketListenerData
{
	int* client_socket;
};

void* thread_callback_socket_listener(void* data);
