#include "promotion_error_msg.h"
#include "die_socket.h"

#include <Windows.h>


void error_msg() {

	const int result = MessageBox(NULL, "Please turn on promotion mode in cmd : ( administrator )! ", MB_OK, MB_OKCANCEL);

	switch (result) {

	case IDOK:

		die_socket();
		break;
	case IDCANCEL:


	default:
		break;
	}
	
}
