#pragma warning(disable:4996)

#include "WriteData.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <exception>


void WriteData_File(const char* buffer) {

	try {

		FILE * file;
		file = fopen("data.txt", "a");
		fprintf(file, buffer);
		fprintf(file, "\n");
		fclose(file);

	}
	catch (std::exception& ex) {

		std::cout << ex.what() << std::endl;
	}
}
