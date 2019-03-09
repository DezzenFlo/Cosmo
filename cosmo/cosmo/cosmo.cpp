#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma warning(disable:4996)

#include <conio.h>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>
#include <iphlpapi.h>
#include <exception>


#include "die_socket.h"
#include "promotion_error_msg.h"
#include "WriteData.h"
#include "sniff_str_packet.h"
#include "search_ip.h"
#include "help.h"
#include "deaunth_attack.h"
#include "arp_attack.h"
#include "p2p.h"
#include "cosmo-vpn.h"
#include "ftp_server.h"
#include "ftp_client.h"
#include "cosmo_rat.h"
#include "url_request.h"
#include "deauth_attack.h"

#include "vpn/vpn_file.h"
//#include "..\cosmo\deauth_header\all_deauth.h"

#define MAX_PACKET_SIZE    0x10000
#define SIO_RCVALL         0x98000001
// Буфер для приёма данных
char Buffer[MAX_PACKET_SIZE]; // 64 Kb
#define PACKSIZE 65536

#define ETH_HW_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define ARP_FRAME_TYPE 0x0806
#define ETHER_HW_TYPE 1
#define IP_PROTO_TYPE 0x0800
#define OP_ARP_REQUEST 2
#define ETH_P_RARP      0x8035

#define DEFAULT_DEVICE "Ethernet Ethernet"

char usage[] = { "send_arp: sends out custom ARP packet. yuri volobuev'97\n\
\tusage: send_arp src_ip_addr src_hw_addr targ_ip_addr tar_hw_addr\n\n" };

SOCKET sock_arp;

struct arp_packet {

	u_char targ_hw_addr[ETH_HW_ADDR_LEN];
	u_char src_hw_addr[ETH_HW_ADDR_LEN];
	u_short frame_type;
	u_short hw_type;
	u_short prot_type;
	u_char hw_addr_size;
	u_char prot_addr_size;
	u_short op;
	u_char sndr_hw_addr[ETH_HW_ADDR_LEN];
	u_char sndr_ip_addr[IP_ADDR_LEN];
	u_char rcpt_hw_addr[ETH_HW_ADDR_LEN];
	u_char rcpt_ip_addr[IP_ADDR_LEN];
	u_char padding[18];

};

//Структура заголовка IP-пакета
typedef struct IPHeader {
	UCHAR   iph_verlen;   // версия и длина заголовка
	UCHAR   iph_tos;      // тип сервиса
	USHORT  iph_length;   // длина всего пакета
	USHORT  iph_id;       // Идентификация
	USHORT  iph_offset;   // флаги и смещения
	UCHAR   iph_ttl;      // время жизни пакета
	UCHAR   iph_protocol; // протокол
	USHORT  iph_xsum;     // контрольная сумма
	ULONG   iph_src;      // IP-адрес отправителя
	ULONG   iph_dest;     // IP-адрес назначения
} IPHeader;

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
char src[10];
char dest[10];
char ds[15];
unsigned short lowbyte;
unsigned short hibyte;


char* getMAC() {
 
	try {

		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
		char *mac_addr = (char*)malloc(18);

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return NULL;
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
			free(AdapterInfo);
			AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
			if (AdapterInfo == NULL) {
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
				free(mac_addr);
				return NULL;
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {

			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
			do {

				sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);

				printf(pAdapterInfo->AdapterName);
				printf("\n");

				printf("\n");
				pAdapterInfo = pAdapterInfo->Next;


			} while (pAdapterInfo);
		}
		free(AdapterInfo);
		return mac_addr;

	}
	catch (std::exception& ex) {

		std::cout << ex.what() << std::endl;
		exit(-1);
	}
}

void process_pak(char *pak, int len) {

	try {

		struct iphdr *ip;
		struct tcphdr *tcp;
		char *data;
		unsigned char proto;
		int datasize;

		ip = (struct iphdr *)pak;
		proto = ip->proto;

		printf("-|Prototype: %i ", (unsigned char)proto);

		if (proto = IPPROTO_TCP) {

			tcp = (struct tcphdr *)(pak + (ip->ihl * 4));
			data = pak + (ip->ihl * 4) + (tcp->tcphl * 4);
			datasize = ntohs(ip->totlen) - (ip->ihl * 4) - (tcp->tcphl * 4);

			char temp[50];

			sprintf(temp, "-|New Packet: %x %x %x %x \n", data[0], data[1], data[2], data[3]);
			puts(temp);

			printf("-|DataSize: %i\n", datasize);
		}
	}
	catch (std::exception& ex) {

		std::cout << ex.what() << std::endl;
		exit(-1);
	}
}

int main(int argc, char *argv[])
{

	WSADATA     wsadata;   // Инициализация WinSock.
	SOCKET      s;         // Cлущающий сокет.
	//char        name[128]; // Имя хоста (компьютера).
	HOSTENT*    phe;       // Информация о хосте.
	SOCKADDR_IN sa;        // Адрес хоста
	IN_ADDR sa1;        //
	unsigned long        flag = 1;  // Флаг PROMISC Вкл/выкл.
	DWORD bytes;

	// инициализация
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	gethostname(argv[1], sizeof(argv[1]));
	phe = gethostbyname(argv[1]);
	ZeroMemory(&sa, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	sa.sin_addr.s_addr = ((struct in_addr *)phe->h_addr_list[0])->s_addr;
	bind(s, (SOCKADDR *)&sa, sizeof(SOCKADDR));

	// Включение promiscuous mode.
	if ((ioctlsocket(s, SIO_RCVALL, &flag)) == SOCKET_ERROR) {

		error_msg();
	}

	char* pMac = getMAC();
	free(pMac);
	system("pause");

	std::string choice;
	std::cout << "#--->>> ";
	std::cin >> choice;

	if (choice == "-deauth_router") {


		start_attack(s);
	}
	else if (choice == "-deauth") {

		//do_deauth_attack();
	}
	else if (choice == "-str_sniff") {

		sniff_str_packet(s);

	}
	else if (choice == "-search") {

		std::string file;
		std::string ip;

		std::cout << "File: ";
		std::cin >> file;

		std::cout << "Ip: ";
		std::cin >> ip;
		printf("\n");

		search_ip(file, ip);
	}
	else if (choice == "-arp") {

		//test

		//test
	}
	else if (choice == "-vpn") {

		std::string str;

		std::cout << "Server(-s) / Client(-c): ";
		std::cin >> str;

		if (str == "-s") {

			send_file();

		}
		else if (str == "-c") {

			start_vpn();
		}
		else {

			printf("You have not entered data()");
			WSACleanup();

			exit(-1);
		}
	}
	else if (choice == "-p2p") {

		start_p2p();
	}
	else if (choice == "-ftp_server") {

		server();
	}
	else if (choice == "-ftp_client") {

		start_client();
	}
	else if (choice == "url") {

		std::string url;

		std::cout << "Url: ";
		std::cin >> url;

		send_url_request(url);

	}
	else if (choice == "-rat") {

		start_rat();
	}
	else if (choice == "--help") {

		print_help();
	}
	else if(choice == "-sniffer"){

		// Бесконечный цикл приёма IP-пакетов.
		while (!_kbhit())
		{
			int count;
			count = recv(s, Buffer, sizeof(Buffer), 0);
			// обработка IP-пакета
			if (count >= sizeof(IPHeader))
			{
				IPHeader* hdr = (IPHeader *)Buffer;
				//Начинаем разбор пакета...

				strcpy(src, "-|Packet: ");
				CharToOem(src, dest);
				printf(dest);

				// Преобразуем в понятный вид адрес отправителя.
				printf("-|From ");
				sa1.s_addr = hdr->iph_src;
				printf(inet_ntoa(sa1));

				// Преобразуем в понятный вид адрес получателя.
				printf(" -|To ");
				sa1.s_addr = hdr->iph_dest;
				printf(inet_ntoa(sa1));


				char *data_write = inet_ntoa(sa1);
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WriteData_File, (LPVOID)(data_write), NULL, NULL);


				// Вычисляем протокол. Полный список этих констант
				// содержится в файле winsock2.h

				printf(" -|Prot: ");
				char pak[PACKSIZE];

				if (hdr->iph_protocol == IPPROTO_TCP) {

					printf("-|TCP ");

					memset(pak, 0, sizeof(pak));

					if ((bytes = recv(s, pak, sizeof(pak), 0)) == SOCKET_ERROR) {

						printf("socket error on recv\n");
						die_socket();

					}
					else {

						process_pak(pak, bytes);
					}

				}
				if (hdr->iph_protocol == IPPROTO_UDP) printf("-|UDP ");
				if (hdr->iph_protocol == IPPROTO_IPV4) printf("-|IPV4 ");
				if (hdr->iph_protocol == IPPROTO_IPV6) printf("-|IPV6 ");
				if (hdr->iph_protocol == IPPROTO_RAW) printf("-|RAW ");

				// Вычисляем размер. Так как в сети принят прямой порядок
				// байтов, а не обратный, то прийдётся поменять байты местами.
				printf("-|Size: ");
				lowbyte = hdr->iph_length >> 8;
				hibyte = hdr->iph_length << 8;
				hibyte = hibyte + lowbyte;
				printf("%s", itoa(hibyte, ds, 10));

				// Вычисляем время жизни пакета.
				printf(" -|TTL:%s", itoa(hdr->iph_ttl, ds, 10));
				printf("\n");

			}
		}
	}
	else {

		printf("Please enter command!");
		die_socket();
		exit(0);
	}

	closesocket(s);
	WSACleanup();
	return 0;
}