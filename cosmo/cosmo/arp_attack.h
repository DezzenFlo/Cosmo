#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

void die(char *);
void get_ip_addr(struct in_addr*, char*);
void get_hw_addr(char*, char*);
