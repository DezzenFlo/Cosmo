#include "vpn_file.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <stdio.h>

const int MAX_CLIENT = 64;

SOCKET Connect_v;
SOCKET Connections_v[MAX_CLIENT];
SOCKET Listen_v;
SOCKADDR_IN name_conn_v[MAX_CLIENT];

int ClientCount_v = 0;

// +----------------- enctypted + file --------------------
char encrypted_file(std::string buffer) {

	std::string enctypted;
	char key = 'X';

	for (int i = 0; i < buffer.size(); i++) {

		enctypted += buffer[i] ^ (int(key) + i) % 20;
	}

	return 'c';
}
// +----------------- enctypted + file --------------------

void vpn_send_file(int ID) {

	char name[256] = "C:\\Users\\maksi\\Desktop\\noip.txt";
	char filename[50];

	int i = strlen(name);

	for (; i > 0; i--) if (name[i - 1] == '\\') break;

	for (int j = 0; j <= (int)strlen(name); i++) filename[j++] = name[i];

	std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);
	int size = (int)file.tellg();
	file.close();

	char filesize[10]; itoa(size, filesize, 10);
	char rec[32] = "";

	send(Connections_v[ID], filesize, strlen(filesize), 0);
	recv(Connections_v[ID], rec, 32, 0);

	FILE * fr = fopen(name, "rb");

	while (size > 0) {

		char buffer[1030];

		if (size >= 1024) {

			fread(buffer, 1024, 1, fr);

			char buff = encrypted_file(buffer);

			send(Connections_v[ID], &buff, 1024, 0);
			recv(Connections_v[ID], rec, 32, 0);

		}
		else {

			fread(buffer, size, 1, fr);
			buffer[size] = '\0';

			char buff = encrypted_file(buffer);

			send(Connections_v[ID], &buff, size, 0);
			recv(Connections_v[ID], rec, 32, 0);
		}

		size -= 1024;
	}

	fclose(fr);
}

void send_file() {

	WSAData data;
	WORD version = MAKEWORD(2, 2);
	int res = WSAStartup(version, &data);
	if (res != 0) {

		printf("Socket()error");

		WSACleanup();
		exit(-1);
	}

	struct addrinfo hints;
	struct addrinfo *result;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char port[10];

	std::cout << "Port: ";
	std::cin >> port;

	getaddrinfo("192.168.0.106", port, &hints, &result);

	Listen_v = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen_v, result->ai_addr, result->ai_addrlen);
	listen(Listen_v, SOMAXCONN);
	freeaddrinfo(result);

	for (;; Sleep(75)) {

		int size = sizeof(name_conn_v[ClientCount_v]);

		if (Connect_v = accept(Listen_v, (SOCKADDR *)&name_conn_v[ClientCount_v], &size)) {

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)vpn_send_file, LPVOID(ClientCount_v - 1), NULL, NULL);
		}
	}

}