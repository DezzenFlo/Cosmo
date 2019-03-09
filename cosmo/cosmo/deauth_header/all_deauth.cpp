#include <all_deauth.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <stdlib.h>

EXPORT int
wi_read(struct wif * wi, unsigned char * h80211, int len, struct rx_info * ri){

	assert(wi->wi_read);
	return wi->wi_read(wi, h80211, len, ri);	
}

EXPORT int
wi_write(struct wif * wi, unsigned char * h80211, int len, struct tx_info * ti){

	assert(wi->wi_write);
	return wi->wi_write(wi, h80211, len, ti);
}

EXPORT int wi_set_chennel(struct wif * wi, int chan){

	assert(wi->wi_set_chennel);
	return wi->wi_set_chennel(wi, chan);
}