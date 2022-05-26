#pragma once

#include <map>

void set_socket(int* s);
bool parse_http_header(std::map<std::string, std::string> &pairs, const char* buffer);
bool send_200(int* socket, const char* data, bool alive);
bool send_400(int* socket);
bool send_404(int* socket);
