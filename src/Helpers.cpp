#include "Helpers.h"
#include "Lua.h"
#include <string>
#include <regex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int* global_socket = nullptr;

void set_socket(int* s)
{
	global_socket = s;
}

bool parse_http_header(std::map<std::string, std::string> &pairs, const char* buffer)
{
	std::string buff(buffer);
	
	std::regex url("GET /\\w+\\.\\w+\\?*[\\w*\\=\\w*&]+");
	std::regex version("HTTP/1.\\d");
	std::regex connection_alive("Connection: keep-alive");

	std::smatch _url;
	std::smatch version_num;
	std::smatch connection_stat;

	std::regex_search(buff, version_num, version);
	std::regex_search(buff, connection_stat, connection_alive);
	std::regex_search(buff, _url, url);	

	if (version_num.empty())
	{
		puts("Version number was empty.");
		return false;
	}

	int http_version = version_num.str().at(7) - 48;
	pairs["version"] = std::to_string(http_version);

	if (connection_stat.empty() && http_version != 1)
	{
		std::regex connection_kill("Connection: Closed");
		std::regex_search(buff, connection_stat, connection_kill);
		if (connection_stat.empty())
		{
			puts("Connection status was empty.");
			return false;
		}
		
		pairs["connection"] = "close";
	}
	else
	{
		pairs["connection"] = "alive";
	}

	std::string trimmed = _url.str();
	
	// Remove "GET /" from the url string
	if (!trimmed.empty())
	{
		trimmed.erase(0, 5);

		if (trimmed.find(".lua") == std::string::npos && trimmed.find(".luac") == std::string::npos)
		{
			if (trimmed.find(".html") == std::string::npos)
			{
				trimmed.append(".html");
			}
			pairs["lua"] = "false";
		}
		else
		{
			// Not entirely true, but for our server it is.
			pairs["lua"] = "true";
		}
	}

	pairs["url"] = _url.empty() ? "index.html" : trimmed;

	if (!trimmed.empty())
	{
		size_t loc = trimmed.find("?");
		
		if (loc != std::string::npos)
		{
			trimmed.erase(loc, trimmed.length() - 1);
		}
	}

	pairs["parsed_url"] = _url.empty() ? "index.html" : trimmed;

	return true;
}

bool send_200(int* socket, const char* data, bool alive)
{
	std::string buffer =    "HTTP/1.1 200 OK\n"
				"Date: Tue, 04 Feb 2018 12:26:30 GMT\n"
				"Server: AHarrisServer/6.0\n"
				"Last-Modified: Tue, 04 Feb 2018 12:26:30 GMT\n"
				"Content-Length: %NUM%\n"
				"Content-Type: text/html\n"
				"Connection: %_CONN%\n\n";

	if (alive)
	{
		buffer.replace(buffer.find("%_CONN%"), std::string("%_CONN%").length(), "keep-alive");
	}
	else
	{
		buffer.replace(buffer.find("%_CONN%"), std::string("%_CONN%").length(), "Closed");
	}

	int size = std::string(data).length();

	buffer.replace(buffer.find("%NUM%"), std::string("%NUM%").length(), std::to_string(size));
	buffer.append(data);

	int* tmp = (socket == nullptr) ? global_socket : socket;
	if (send(*tmp, (void*)buffer.c_str(), buffer.length(), 0) == -1)
	{
		return false;
	}
	
	return true;
}

bool send_400(int* socket)
{
	char* buffer =  "HTTP/1.1 400 Bad Request\n"
			"Date: Sun, 10 Feb 2018 10:30:20 GMT\n"
			"Server: AHarrisServer/6.0\n"
			"Content-Length: 309\n"
			"Content-Type: text/html; charset=iso-8859-1\n"
			"Connection: Closed\n\n"
			"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
			"<html>\n"
			"\n"
			"<head>\n"
			"   <title>400 Bad Request</title>\n"
			"</head>\n"
			"\n"
			"<body>\n"
			"   <h1>Bad Request</h1>\n"
			"   <p>Your browser sent a request that this server could not understand.<p>\n"
			"   <p>The request line contained invalid characters following the protocol string.<p>\n"
			"</body>\n"
			"\n"
			"</html>";

	int size = std::string(buffer).length();

	if (send(*socket, (void*)buffer, size, 0) == -1)
	{
		return false;
	}

	return true;
}

bool send_404(int* socket)
{
	char* buffer =  "HTTP/1.1 404 Not Found\n"
		        "Content-Length: 211\nContent-Type: text/html\n"
		        "Server: AHarrisServer/6.0\n"
		        "X-Powered-By: AHarris\n"
		        "Date: Tue, 04 Feb 2018 12:26:30 GMT\n"
		        "Connection: Closed\n\n"
		        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
			"<html>\n"
			"\n"
			"<head>\n"
			"   <title>404 Not Found</title>\n"
			"</head>\n"
			"\n"
			"<body>\n"
			"   <h1>Not Found</h1>\n"
			"   <p>The requested URL was not found on this server.</p>\n"
			"</body>\n"
			"\n"
			"</html>\n";

	int size = std::string(buffer).length();

	if (send(*socket, (void*)buffer, size, 0) == -1)
	{
		return false;
	}

	return true;
}
