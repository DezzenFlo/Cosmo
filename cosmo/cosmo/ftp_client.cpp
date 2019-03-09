#include "ftp_client.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

#include"die_socket.h"

SOCKET Client_c;

void printProcess() {

	float progress = 0.0;

	while (progress < 1.0) {
		int barWidth = 70;

		std::cout << "[";
		int pos = barWidth * progress;
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();

		progress += 0.16; // for demonstration only
	}
	std::cout << std::endl;
}


void start_client() {

	int port;
	char adrserv[INET_ADDRSTRLEN];

	WSAData data;
	WORD versions = MAKEWORD(2, 2);
	int res = WSAStartup(versions, &data);
	if (res != 0) {

		printf("Socket()error");
		die_socket();
		exit(-1);
	}

	Client_c = socket(AF_INET, SOCK_STREAM, 0);
	if (Client_c < 0) {

		printf("Socket()error");
		die_socket();
		exit(-1);
	}

	printf("Ip: ");
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
			closesocket(Client_c);
			die_socket();
			
			exit(-1);
		}
	}

	if (connect(Client_c, (sockaddr *)&connect_c, sizeof(connect_c))) {

		printf("Connect()error");
		die_socket();

		exit(-1);
	}

	printf("Connect is {+- OK -+}\n");

	//char *buffer = new char[1024];
	//recv(Client_c, buffer, strlen(buffer), 0);

	//printf(buffer);


	//Password---------------------------------------------------#

	std::string password;

	std::cout << "Password: ";
	std::cin >> password;

	//Password---------------------------------------------------#

	if (password == "root") {

		char text[256];
		char infile[80];
		memset(infile, 0, sizeof(infile));

		char rec[50] = "";

		recv(Client_c, infile, sizeof(infile) - 1, 0);
		wsprintf(text, "Начато принятия файла %s", infile);

		send(Client_c, "OK", strlen("OK"), 0);

		FILE * file = fopen("config.txt", "wb");

		int recs = recv(Client_c, rec, 32, 0);
		send(Client_c, "OK", strlen("OK"), 0);

		printProcess();

		rec[recs + 1] = '\0';
		int size = atoi(rec);

		while (size > 0) {

			char buffer[1030];

			if (size >= 1024) {

				std::string decrypted;
				std::string buff;
				char key = 'X';

				recv(Client_c, buffer, 1024, 0);
				send(Client_c, "OK", strlen("OK"), 0);

				buff = buffer;

				for (int i = 0; i < buff.size(); i++) {

					decrypted += buff[i] ^ (int(key) + i) % 20;

				}

				fwrite(decrypted.c_str(), 1024, 1, file);

			}
			else {

				recv(Client_c, buffer, size, 0);
				send(Client_c, "OK", strlen("OK"), 0);
				buffer[size] = '\0';

				std::string decrypted;
				std::string buff;
				char key = 'X';

				buff = buffer;

				for (int i = 0; i < buff.size(); i++) {

					decrypted += buff[i] ^ (int(key) + i) % 20;

				}
				fwrite(buffer, size, 1, file);
				printProcess();

			}

			size -= 1024;

		}

		fclose(file);

		printf("Acceptance is Complete(---+---)\n");

	}
	else if (password != "root") {

		die_socket();
		exit(-1);
	}

}
