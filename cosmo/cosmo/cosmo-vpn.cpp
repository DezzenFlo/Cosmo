#include "cosmo-vpn.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <fstream>

#include "die_socket.h"

const int MAX_CLIENT = 64;

SOCKET sock;
SOCKET Listen_s;
SOCKET Connect_s;
SOCKET Connection[MAX_CLIENT];
SOCKADDR_IN name_conn[MAX_CLIENT];

int ClientCount_s = 0;

void SendMessageToClient_s(int ID) {

	char *buffer = new char[1024];
	char key = 'X';

	std::string message;
	std::string out;

	for (;; Sleep(75)) {

		for (int i = 0; i < 1024; i++) buffer[i] = 0;

		if (recv(Connection[ID], buffer, 1024, 0) > 0) {

			out = buffer;

			for (int j = 0; j < out.size(); j++) {

				message += out[j] ^ (int(key) + j) % 20;
			}

			printf(message.c_str());
			printf("\n");

		}
		else break;
	}

	ClientCount_s--;
	delete buffer;
}

void start_vpn() {

	WSAData data;
	WORD version = MAKEWORD(2, 2);
	int res = WSAStartup(version, &data);
	if (res != 0) {

		printf("Socket()error");

		die_socket();
		exit(-1);
	}

	int port;
	char message[1024];
	char in[1024];

	std::string encrypted_message;
	std::string input;

	char key = 'X';


	char adrserv[INET_ADDRSTRLEN];

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {

		printf("Socket()erorr");
		die_socket();
		exit(-1);
	}

	std::cout << "Ip: ";
	std::cin >> adrserv;

	std::cout << "Port: ";
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

			printf("Invlid address %s\n", adrserv);
			closesocket(sock);
			die_socket();

			exit(-1);
		}
	}

	if (connect(sock, (sockaddr *)&connect_c, sizeof(connect_c))) {

		printf("Connct()error");
		die_socket();

		exit(-1);

	}

	printf("{OK}");
	printf(" -port: ");
	printf("5555");

	do {

		for (int i = 0; i < 1024; i++) message[i] = 0;

		fflush(stdin);
		gets_s(in);

		strcat(message, in);

		send(sock,encrypted_message.c_str(), strlen(encrypted_message.c_str()), 0);

	} while (1);

	closesocket(sock);
	die_socket();
	exit(-1);

}


