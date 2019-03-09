#include "url_request.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <sstream>
#include <locale>

#include "die_socket.h"

void send_url_request(std::string url) {

	WSAData data;
	WORD version = MAKEWORD(2, 2);
	int res = WSAStartup(version, &data);
	if (res != 0) {

		printf("Socket()error");

		die_socket();
		exit(-1);
	}

	SOCKADDR_IN sock_addr;
	SOCKET sock_url;
	int lineCount = 0;
	int rowCount = 0;
	struct hostent *host;
	std::locale local;
	char buffer[10000];
	int DataLength;
	std::string website_HTML;

	std::string site_url = url;

	std::string get_http = "GET / HTTP/1.1\r\nHost: " + site_url + "\r\nConnection: close\r\n\r\n";

	sock_url = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	host = gethostbyname(site_url.c_str());

	sock_addr.sin_port = htons(80);
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(sock_url, (SOCKADDR*)(&sock_addr), sizeof(sock_addr)) != 0) {

		printf("Connect()error");

		die_socket();
		exit(-1);
	}

	send(sock_url, get_http.c_str(), strlen(get_http.c_str()), 0);

	while ((DataLength = recv(sock_url, buffer, 10000, 0)) > 0) {

		int i = 0;

		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {

			website_HTML += buffer[i];
			i += 1;
		}
	}

	closesocket(sock_url);
	die_socket();

	std::cout << website_HTML;
}

