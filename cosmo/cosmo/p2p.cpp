#include "p2p.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <string>
#include <exception>

#include "die_socket.h"

const int MAX_CLIENTS = 64;

SOCKET Connect;
SOCKET Client;
SOCKET Connections[MAX_CLIENTS];
SOCKET Listen;
SOCKADDR_IN name_conn_s[MAX_CLIENTS];

int ClientCount = 0;

void p2pClient() {

	char key = 'X';
	std::string encrypted;
	std::string buffer;

	int port;
	char message[1024];
	char in[1024];
	char adrserv[INET_ADDRSTRLEN];

	WSAData data;
	WORD versions = MAKEWORD(2, 2);
	int res = WSAStartup(versions, &data);
	if (res != 0) {

		printf("Socket(-|-)error");
		die_socket();
		exit(-1);
	}

	Client = socket(AF_INET, SOCK_STREAM, 0);
	if (Client < 0) {

		printf("Socket()error");
		die_socket();
		exit(-1);

	}

	printf("Ip: \n");
	std::cin >> adrserv;

	printf("Port: ");
	std::cin >> port;


	struct sockaddr_in connect_c;

	connect_c.sin_family = AF_INET;
	connect_c.sin_port = htons(port);

	HOSTENT *hst;

	if (inet_addr(adrserv) != INADDR_NONE)
		connect_c.sin_addr.s_addr = inet_addr(adrserv);

	else {

		if (hst = gethostbyname(adrserv))

			((unsigned long *)&connect_c.sin_addr)[0] =
			((unsigned long **)&hst->h_addr_list)[0][0];

		else {

			printf("Invalid address %s\n", adrserv);
			closesocket(Client);
			die_socket();

			exit(-1);
		}
	}

	if (connect(Client, (sockaddr *)&connect_c, sizeof(connect_c))) {

		printf("Connect()error");
		die_socket();
		exit(-1);
	}

	do {

		for (int i = 0; i < 1024; i++) message[i] = 0;

		fflush(stdin);
		gets_s(in);

		strcat(message, in);

		buffer = message;

		for (int i = 0; i < buffer.size(); i++) {

			encrypted += buffer[i] ^ (int(key) + i) % 20;
		}

		send(Client, buffer.c_str(), strlen(buffer.c_str()), 0);


	} while (1);
}

void SendMessageToClient(int ID) {

	std::string message;
	char *buffer = new char[1024];
	std::string decrypted;
	char key = 'X';

	for (;; Sleep(75)) {

		for (int i = 0; i < 1024; i++) buffer[i] = 0;

		if (recv(Connections[ID], buffer, 1024, 0) > 0) {

			message = buffer;

			for (int i = 0; i < message.size(); i++) {

				decrypted += message[i] ^ (int(key) + i) % 20;
			}

			printf(decrypted.c_str());
		}
	}

	delete buffer;
	ClientCount--;
}

void start_p2p() {

	WSAData data;
	char port[10];

	WORD versions = MAKEWORD(2, 2);
	int res = WSAStartup(versions, &data);
	if (res != 0) {

		printf("Socket(-|-)error!");
		die_socket();
		exit(-1);
	}

	struct addrinfo hints;
	struct addrinfo *result;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	printf("Port: ");
	std::cin >> port;

	getaddrinfo(NULL, port, &hints, &result);
	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);
	listen(Listen, SOMAXCONN);
	freeaddrinfo(result);

	printf("#--->\t Server Started(-|-)");

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)p2pClient, CreateMutex(NULL, FALSE, NULL), NULL, NULL);

	for (;; Sleep(75)) {

		int size = sizeof(name_conn_s[ClientCount]);

		if (Connect = accept(Listen, (SOCKADDR *)&name_conn_s[ClientCount], &size)) {

			Connections[ClientCount] = Connect;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient, (LPVOID)(ClientCount - 1), NULL, NULL);
		}
	}

}