#include "search_ip.h"

#include <fstream>
#include <Windows.h>
#include <string>
#include <iostream>

#include "die_socket.h"

void search_ip(std::string file, std::string ip) {

	std::ifstream ip_file(file);
	char buffer[50];
	int count = 0;

	while (ip_file.getline(buffer, 50)) {

		if (buffer == ip) {

			printf("Found ip: ---------> ");
			printf(ip.c_str());
			printf("\n");

			die_socket();
			exit(0);

		}
		else if (buffer != ip) {

			count++;
			continue;
		}
	}


	die_socket();
	exit(0);
}
