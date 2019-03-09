#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>

void blocking_traffic(SOCKET sock, std::string ip);
