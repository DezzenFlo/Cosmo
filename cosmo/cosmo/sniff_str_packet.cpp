#include "sniff_str_packet.h"
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <exception>

#include "die_socket.h"

void sniff_str_packet(SOCKET sock) {

	setlocale(LC_ALL, "rus");

	try {

		char *buffer = new char[1024];

		for (;; Sleep(75)) {

			for (int i = 0; i < 1024; i++) buffer[i] = 0;

			if (recv(sock, buffer, 1024, 0) > 0) {

				printf(buffer);

			}
		}

		delete buffer;
		WSACleanup();
		exit(1);

	}
	catch (std::exception& ex) {

		std::cout << ex.what() << std::endl;
		die_socket();
		exit(-1);
	}
}
