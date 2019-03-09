#include <assert.h>
#include <iostream>
#include <stdint.h>

struct tx_info{

	uint32_t ti_rate;

};

struct rx_info{

	uint64_t ri_mactime;
	int32_t ri_power;
	int32_t ri_noise;
	uint32_t ri_channel;
	uint32_t ri_freq;
	uint32_t ri_rate;
	uint32_t ri_antenna;

} __packed;

struct wif{

	int(*wi_write)(struct wif * wi, unsigned char * h80211, int len, struct tx_info * ti);

	int (*wi_set_channel)(struct wif * wi, int chan);
	int (*wi_get_channel)(struct wif * wi);
};

//int wi_write(struct wif * wi, unsigned char * h80211, int len, struct tx_info * ti);