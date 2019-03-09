#include "die_socket.h"
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>


void die_socket() {
	
	WSACleanup();
	exit(-1);

}
