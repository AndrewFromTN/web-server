#include "ThreadFunctions.h"
#include "Helpers.h"
#include "Lua.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h> 
#include <fstream>
#include <streambuf>
#include <sstream>

void* thread_callback_socket_listener(void* data)
{
	struct SocketListenerData* socket_data = (struct SocketListenerData*)data;

	while (true)
	{
		char* buffer = new char[700];
		std::map<std::string, std::string> header_info;

		long data_size = recv(*socket_data->client_socket, (void*)buffer, 700, 0);

		// Client Disconnected
		if (data_size == 0)
		{
			puts("A client disconnected.");
			
			delete[] buffer;
			buffer = nullptr;

			break;
		} 
		else if (data_size == -1)
		{
			puts("Failed to read from socket. errno: " + errno);
			
			delete[] buffer;
			buffer = nullptr;
			continue;
		}

		if (!parse_http_header(header_info, const_cast<char*>(buffer)))
		{
			puts("Failed to read a header. Sending message.");
			if (!send_400(socket_data->client_socket))
			{
				puts("Failed to send 400. errno: " + errno);
			}

			delete[] buffer;
			buffer = nullptr;

			// We don't know if he wanted to keep-alive since
			// the header was corrupted. We're going to kill it.
			break;
		}

		std::ifstream file;
		file.open("./" + header_info["parsed_url"]);

		if (file.fail())
		{
			if (!send_404(socket_data->client_socket))
			{
				puts("Failed to send 404. errno: " + errno);
			}

			delete[] buffer;
			buffer = nullptr;

			if (header_info["connection"] == "alive" || atoi(header_info["version"].c_str()) == 1)

				continue;
			else
				break;
		}
		
		bool alive = header_info["connection"] == "alive" ? true : false;
		if (header_info["lua"] == "true")
		{
			set_socket(socket_data->client_socket);

			std::map<std::string, std::string> arguments;
			if (header_info["url"].find("?"))
			{
				std::istringstream iss(header_info["url"]);
				std::string url;
				if(!std::getline(iss, url, '?'))
				{
					puts("Error parsing parameters.");	
				}

				std::string keyval, key, val;

				while(std::getline(iss, keyval, '&'))
				{
					std::istringstream ss(keyval);

					if(std::getline(std::getline(ss, key, '='), val))
					{
				    		arguments[key] = val;
					}
				}
			}
			
			if (!execute_script("./" + header_info["parsed_url"], arguments))
			{
				puts("Failed to execute script.");
			}
		}
		else
		{
			std::string buff;
			if (file.is_open())
			{
				buff = std::string((std::istreambuf_iterator<char>(file)),
		         	std::istreambuf_iterator<char>());

	   			file.close();
			}

			if (!send_200(socket_data->client_socket, buff.c_str(), alive))
			{
				puts("Failed to send 200 with data. errno: " + errno);
			}
		}

		delete[] buffer;
		buffer = nullptr;

		if (!alive)
			break;
	}

	return nullptr;
}

