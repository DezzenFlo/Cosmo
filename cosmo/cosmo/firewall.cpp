#include "firewall.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>

#include "die_socket.h"

void blocking_traffic(SOCKET sock, std::string ip) {

	const int result = MessageBox(NULL, ip.c_str(), MB_OK, MB_OKCANCEL);

	char buffer[] = "Доступ разрешён{200/OK}";

	char buffer_error[] = "Доступ запрешён{404/OK}";

	switch (result) {

	case IDOK:

		send(sock, buffer, strlen(buffer), 0);
		break;

	case IDCANCEL:

		send(sock, buffer_error, strlen(buffer_error), 0);

		Sleep(1000);

		closesocket(sock);
		die_socket();

		break;

	default:

		closesocket(sock);
		die_socket();

		MessageBox(NULL, "Cosmo-Firewall " ,MB_OK, MB_OKCANCEL);
		break;
	}
}