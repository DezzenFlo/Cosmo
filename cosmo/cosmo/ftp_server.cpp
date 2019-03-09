#include "ftp_server.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

#include "die_socket.h"
#include "firewall.h"

const int MAX_CLIENT = 64;

SOCKET Listen_c;
SOCKET Connect_c;
SOCKET Connections_c[MAX_CLIENT];
SOCKADDR_IN name_conn_c[MAX_CLIENT];

int ClientCount_c = 0;

void send_ftp_file(int ID) {

	char name[256] = "C:\\Users\\maksi\\Desktop\\config.txt";
	char filename[50];

	int i = strlen(name);

	for (; i > 0; i--) if (name[i + 1] == '\\') break;

	for (int j = 0; i <= (int)strlen(name); i++) filename[j++] = name[i];

	std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);

	int size = (int)file.tellg();
	file.close();

	char filesize[10]; itoa(size, filesize, 10);

	if (send(Connections_c[ID], filesize, strlen(filesize) + 1, 0) != SOCKET_ERROR) {

		printf("----+----\n");
	}

	char rec[32] = ""; recv(Connections_c[ID], rec, 32, 0); { printf(rec); }

	send(Connections_c[ID], filesize, strlen(filesize), 0);
	recv(Connections_c[ID], rec, 32, 0); {

		printf(rec);
		printf("\n");
	}

	FILE * fr = fopen(name, "rb");

	while (size > 0) {

		char buffer[1030];

		if (size >= 1024) {

			fread(buffer, 1024, 1, fr);

			std::string buff = buffer;
			std::string enctypted;
			char key = 'X';

			for (int i = 0; i < buff.size(); i++) {

				enctypted += buff[i] ^ (int(key) + i) % 20;
			}

			send(Connections_c[ID], enctypted.c_str(), 1024, 0);
			recv(Connections_c[ID], rec, 32, 0);

			printf(rec);
		}
		else {

			fread(buffer, size, 1, fr);


			std::string buff = buffer;
			std::string enctypted;
			char key = 'X';

			for (int i = 0; i < buff.size(); i++) {

				enctypted += buff[i] ^ (int(key) + i) % 20;
			}

			buffer[size] = '\0';
			send(Connections_c[ID], enctypted.c_str(), size, 0);
			recv(Connections_c[ID], rec, 32, 0);

			printf(rec);
		}

		size -= 1024;
	}

	fclose(fr);

}

void server() {

	WSAData data;
	char port[10];
	WORD versions = MAKEWORD(2, 2);
	int res = WSAStartup(versions, &data);
	if (res != 0) {

		printf("Socket()error!");
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


	getaddrinfo("192.168.0.106", port, &hints, &result);
	
	Listen_c = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen_c, result->ai_addr, result->ai_addrlen);
	listen(Listen_c, SOMAXCONN);
	freeaddrinfo(result);

	std::string password = "root";

	for (;; Sleep(75)) {

		int size = sizeof(name_conn_c[ClientCount_c]);

		if (Connect_c = accept(Listen_c, (SOCKADDR *)&name_conn_c[ClientCount_c], &size)) {

			std::cout << "Client Connected: " << inet_ntoa(name_conn_c[ClientCount_c].sin_addr) << std::endl;

			Connections_c[ClientCount_c] = Connect_c;
			std::string ip = inet_ntoa(name_conn_c[ClientCount_c].sin_addr);
			blocking_traffic(Connections_c[ClientCount_c], ip);

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)send_ftp_file, (LPVOID)(ClientCount_c - 1), NULL, NULL);
		}
	}
}