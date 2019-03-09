#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define PCAP_ERRBUF_SIZE 256

typedef struct pcap pcap_t;


void send_packet(SOCKET sock_cl, u_char *buff, int size);
