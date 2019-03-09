#include "database.h"
#include "die_socket.h"

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdint.h>


void database(std::string process_id) {
	
	std::ifstream database_file("database.txt");
	char buffer[50];

	while (database_file.getline(buffer, 50)) {

		if (buffer == process_id) {

			const int result = MessageBox(NULL, buffer, MB_OK, MB_OKCANCEL);

			switch (result) {

			case IDOK:

				database_file.close();
				die_socket();
				break;

			case IDCANCEL:

			default:

				break;
			}
		}
		else if (buffer != process_id) {

			continue;
		}
	}
	
}